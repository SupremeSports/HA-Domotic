"""Support for tracking the moon phases."""
from __future__ import annotations
from datetime import datetime, timedelta
import logging

import ephem
from ephem import degree
import voluptuous as vol

from homeassistant import util
from homeassistant.components.sensor import (
    PLATFORM_SCHEMA,
    SensorEntity,
    SensorEntityDescription,
)
from homeassistant.const import (
    ATTR_UNIT_OF_MEASUREMENT,
    CONF_NAME,
    DEGREE,
    ILLUMINANCE,
    PERCENTAGE,
) 
import homeassistant.helpers.config_validation as cv
from homeassistant.util.dt import as_local, get_time_zone, utcnow
from homeassistant.util.unit_system import METRIC_SYSTEM

_LOGGER = logging.getLogger(__name__)

DEFAULT_NAME = "Moon"

DEVICE_CLASS_MOONPHASE = "moon__phase"

STATE_PHASE = "phase"

STATE_FIRST_QUARTER = "first_quarter"
STATE_FULL_MOON = "full_moon"
STATE_LAST_QUARTER = "last_quarter"
STATE_NEW_MOON = "new_moon"
STATE_WANING_CRESCENT = "waning_crescent"
STATE_WANING_GIBBOUS = "waning_gibbous"
STATE_WAXING_GIBBOUS = "waxing_gibbous"
STATE_WAXING_CRESCENT = "waxing_crescent"

MOON_ICONS = {
    STATE_FIRST_QUARTER: "mdi:moon-first-quarter",
    STATE_FULL_MOON: "mdi:moon-full",
    STATE_LAST_QUARTER: "mdi:moon-last-quarter",
    STATE_NEW_MOON: "mdi:moon-new",
    STATE_WANING_CRESCENT: "mdi:moon-waning-crescent",
    STATE_WANING_GIBBOUS: "mdi:moon-waning-gibbous",
    STATE_WAXING_CRESCENT: "mdi:moon-waxing-crescent",
    STATE_WAXING_GIBBOUS: "mdi:moon-waxing-gibbous",
}

STATE_ABOVE_HORIZON = "above_horizon"
STATE_BELOW_HORIZON = "below_horizon"

STATE_ATTR_AZIMUTH = "azimuth"
STATE_ATTR_CONSTEL = "constellation"
STATE_ATTR_DECLINATION = "declination"
STATE_ATTR_DISTANCE = "distance"
STATE_ATTR_ELEVATION = "elevation"
STATE_ATTR_ELONGATION = "moon_sun_elongation"
STATE_ATTR_HLAT = "heliocentric_latitude"
STATE_ATTR_HLONG = "heliocentric_longitude"
STATE_ATTR_POSITION = "position"
STATE_ATTR_ILLUMINATION = "illumination"
STATE_ATTR_LIBRATION_LAT = "libration_lat"
STATE_ATTR_LIBRATION_LONG = "libration_long"
STATE_ATTR_LUNATION = "lunation"
STATE_ATTR_MAGNITUDE = "magnitude"
STATE_ATTR_NEXT_FIRST_QRT = "next_first_quarter"
STATE_ATTR_NEXT_FULL = "next_full_moon"
STATE_ATTR_NEXT_LAST_QRT = "next_last_quarter"
STATE_ATTR_NEXT_NEW = "next_new_moon"
STATE_ATTR_NEXT_RISING = "next_rising"
STATE_ATTR_NEXT_TRANSIT = "next_transit"
STATE_ATTR_NEXT_SETTING = "next_setting"
STATE_ATTR_CURRENT_RISING = "current_rising"
STATE_ATTR_CURRENT_TRANSIT = "current_transit"
STATE_ATTR_CURRENT_SETTING = "current_setting"
STATE_ATTR_RADIUS = "radius"
STATE_ATTR_RIGHT_ASC = "right_ascension"
STATE_ATTR_RISING = "rising"

ATTR_VISIBILITY = "visibility"
ATTR_ASTRONOMY = "astronomy"
ATTR_MOONDAY = "moon_day"
ATTR_NEXTDATES = "next_dates"

ATTR_LAST_UPDATED = "last_updated"

CNST_AU = 1.496e+8
CNST_MI = 0.621371

CNST_PRECISION = 2

SCAN_INTERVAL = timedelta(seconds=30)

PLATFORM_SCHEMA = PLATFORM_SCHEMA.extend(
    {vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string}
)


async def async_setup_platform(hass, config, async_add_entities, discovery_info=None):
    """Set up the Moon sensor."""
    if None in (hass.config.latitude, hass.config.longitude, hass.config.elevation):
        _LOGGER.error(
            "Latitude, longitude and/or elevation not set in Home Assistant config"
        )
        return False

    time_zone = hass.config.time_zone

    latitude = util.convert(hass.config.latitude, float)
    longitude = util.convert(hass.config.longitude, float)
    elevation = util.convert(hass.config.elevation, float)
    is_metric = hass.config.units is METRIC_SYSTEM


    name = config.get(CONF_NAME)
    
    # _LOGGER.warning(
    #         "Latitude: %s, Longitude: %s, Elevation: %s, Unit System: %s, Time zone: %s", 
    #         latitude, longitude, elevation, is_metric, time_zone
    #     )

    if time_zone is None:
        _LOGGER.warning(
            "Time zone not set in Home Assistant configuration, UTC will be returned"
        )

    if is_metric is None:
        _LOGGER.warning(
            "Unit system not set in Home Assistant configuration, metric will be returned"
        )

    moon_data = MoonData(latitude, longitude, elevation, time_zone, is_metric)
    await moon_data.async_update()

    async_add_entities([MoonSensor(moon_data, name)], True)


class MoonSensor(SensorEntity):
    """Representation of a Moon sensor."""

    def __init__(self, moon_data, name):
        """Initialize the moon sensor."""
        self._name = name
        self._state = None
        self.moon_data = moon_data
        self.datetime = None

    @property
    def name(self):
        """Return the name of the entity."""
        return self._name

    @property
    def device_class(self):
        """Return the device class of the entity."""
        return DEVICE_CLASS_MOONPHASE

    @property
    def extra_state_attributes(self):
        """Return the state attributes."""
        data = {}

        visibility = {
            STATE_ATTR_LUNATION: self.moon_data.data[STATE_ATTR_LUNATION],
            STATE_ATTR_ILLUMINATION: self.moon_data.data[STATE_ATTR_ILLUMINATION],
            STATE_ATTR_MAGNITUDE: self.moon_data.data[STATE_ATTR_MAGNITUDE],
            STATE_ATTR_DISTANCE: self.moon_data.data[STATE_ATTR_DISTANCE],
            STATE_ATTR_RADIUS: self.moon_data.data[STATE_ATTR_RADIUS],
            STATE_ATTR_POSITION: self.moon_data.data[STATE_ATTR_POSITION],
            STATE_ATTR_RISING: self.moon_data.data[STATE_ATTR_RISING],
            STATE_ATTR_LIBRATION_LAT: self.moon_data.data[STATE_ATTR_LIBRATION_LAT],
            STATE_ATTR_LIBRATION_LONG: self.moon_data.data[STATE_ATTR_LIBRATION_LONG],
        }

        astronomy = {
            STATE_ATTR_CONSTEL: self.moon_data.data[STATE_ATTR_CONSTEL],
            STATE_ATTR_AZIMUTH: self.moon_data.data[STATE_ATTR_AZIMUTH],
            STATE_ATTR_ELEVATION: self.moon_data.data[STATE_ATTR_ELEVATION],
            STATE_ATTR_RIGHT_ASC: self.moon_data.data[STATE_ATTR_RIGHT_ASC],
            STATE_ATTR_DECLINATION: self.moon_data.data[STATE_ATTR_DECLINATION],
            STATE_ATTR_ELONGATION: self.moon_data.data[STATE_ATTR_ELONGATION],
            STATE_ATTR_HLAT: self.moon_data.data[STATE_ATTR_HLAT],
            STATE_ATTR_HLONG: self.moon_data.data[STATE_ATTR_HLONG],
        }

        moon_day = {
            STATE_ATTR_CURRENT_RISING: self.moon_data.data[STATE_ATTR_CURRENT_RISING],
            STATE_ATTR_CURRENT_TRANSIT: self.moon_data.data[STATE_ATTR_CURRENT_TRANSIT],
            STATE_ATTR_CURRENT_SETTING: self.moon_data.data[STATE_ATTR_CURRENT_SETTING],
        }

        next_dates = {
            STATE_ATTR_NEXT_RISING: self.moon_data.data[STATE_ATTR_NEXT_RISING],
            STATE_ATTR_NEXT_TRANSIT: self.moon_data.data[STATE_ATTR_NEXT_TRANSIT],
            STATE_ATTR_NEXT_SETTING: self.moon_data.data[STATE_ATTR_NEXT_SETTING],
            STATE_ATTR_NEXT_NEW: self.moon_data.data[STATE_ATTR_NEXT_NEW],
            STATE_ATTR_NEXT_FIRST_QRT: self.moon_data.data[STATE_ATTR_NEXT_FIRST_QRT],
            STATE_ATTR_NEXT_FULL: self.moon_data.data[STATE_ATTR_NEXT_FULL],
            STATE_ATTR_NEXT_LAST_QRT: self.moon_data.data[STATE_ATTR_NEXT_LAST_QRT],
        }

        data[ATTR_VISIBILITY] = visibility
        data[ATTR_ASTRONOMY] = astronomy
        data[ATTR_MOONDAY] = moon_day
        data[ATTR_NEXTDATES] = next_dates
        data[ATTR_LAST_UPDATED] = self.moon_data.data[ATTR_LAST_UPDATED]

        return data

    @property
    def native_value(self):
        """Return the state of the device."""
        return self._state

    @property
    def icon(self):
        """Icon to use in the frontend, if any."""
        return MOON_ICONS.get(self.state)

    async def async_update(self):
        """Get the latest data from Moon and update the state."""
        await self.moon_data.async_update()
        self._state = self.moon_data.data[STATE_PHASE]


class MoonData:
    """Calculate the current moon data."""

    def __init__(self, latitude, longitude, elevation, time_zone, is_metric):
        """Initialize the data object."""

        self.latitude = latitude
        self.longitude = longitude
        self.elevation = elevation
        self.time_zone = time_zone
        self.is_metric = is_metric
        self.datetime = None
        self._data = {}

    async def async_update(self):
        """Get the latest data from moon."""
        # Update data
        self.datetime = datetime.utcnow() + timedelta(hours=0)
        self._data = get_moon(self)


def get_moon(self):
    """Calculate the current moon data."""

    date = self.datetime
    g = ephem.Observer()
    g.name='Home'
    g.lat = str(self.latitude)
    g.lon = str(self.longitude)
    g.elevation = int(self.elevation)
    g.date = date

    # Turn off refraction
    g.pressure = 0

    data = {}

    # Determine Moon values that are not geolocation dependent
    m = ephem.Moon(g.date)

    previous_new_moon = ephem.previous_new_moon(g.date)
    next_first_quarter = ephem.next_first_quarter_moon(g.date) 
    next_full_moon = ephem.next_full_moon(g.date)
    next_last_quarter = ephem.next_last_quarter_moon(g.date) 
    next_new_moon = ephem.next_new_moon(g.date)

    lunation = g.date-previous_new_moon
    phase = get_phase(lunation)
    illumination = m.phase
    magnitude = m.mag
    distance = m.earth_distance * CNST_AU
    # Set distance in miles if HA configuration is not metric
    if not self.is_metric:
        distance *= CNST_MI
    declination = m.dec / degree
    right_ascension = get_hms(m.ra)
    elongation = m.elong / degree
    hlat = m.hlat / degree
    hlon = m.hlon / degree
    radius = m.radius / degree
    constellation = ephem.constellation(m)[1]

    # Determine Moon values that are geolocation dependent
    next_rising = g.next_rising(m)
    next_transit = g.next_transit(m)
    next_setting = g.next_setting(m)
    prev_rising = g.previous_rising(m)
    prev_transit = g.previous_transit(m)
    azimuth = ephem.Moon(g).az / degree
    elevation = ephem.Moon(g).alt / degree
    declination = ephem.Moon(g).dec / degree
    libration_lat = ephem.Moon(g).libration_lat / degree
    libration_long = ephem.Moon(g).libration_long / degree

    # Determine current Moon position
    transit = prev_transit if (next_transit > next_setting) else next_transit
    above_horiz = elevation > 0
    position = STATE_ABOVE_HORIZON if above_horiz else STATE_BELOW_HORIZON
    rising = position is STATE_ABOVE_HORIZON and g.date < transit

    # Determine current 'moon day' if Moon is above horizon, otherwise return next 'moon day' data
    current_rising = prev_rising if next_rising > next_transit else next_rising
    current_transit = prev_transit if (position is STATE_ABOVE_HORIZON and not rising) else next_transit
    current_setting = next_setting

    self.data = {
        STATE_PHASE: phase,
        # Visibility data
        STATE_ATTR_LUNATION: round(lunation, CNST_PRECISION),
        STATE_ATTR_ILLUMINATION: round(illumination, CNST_PRECISION),
        STATE_ATTR_MAGNITUDE: round(magnitude, CNST_PRECISION),
        STATE_ATTR_DISTANCE: round(distance, CNST_PRECISION),
        STATE_ATTR_RADIUS: round(radius, CNST_PRECISION),
        STATE_ATTR_POSITION: position,
        STATE_ATTR_RISING: rising,
        STATE_ATTR_LIBRATION_LAT: round(libration_lat, CNST_PRECISION),
        STATE_ATTR_LIBRATION_LONG: round(libration_long, CNST_PRECISION),

        # Astronomy data
        STATE_ATTR_CONSTEL: constellation,
        STATE_ATTR_AZIMUTH: round(azimuth, CNST_PRECISION),
        STATE_ATTR_ELEVATION: round(elevation, CNST_PRECISION),
        STATE_ATTR_RIGHT_ASC: right_ascension,
        STATE_ATTR_DECLINATION: round(declination, CNST_PRECISION),
        STATE_ATTR_ELONGATION: round(elongation, CNST_PRECISION),
        STATE_ATTR_HLAT: round(hlat, CNST_PRECISION),
        STATE_ATTR_HLONG: round(hlon, CNST_PRECISION),

        # Moon day data
        STATE_ATTR_CURRENT_RISING: ephem.localtime(current_rising).isoformat(),
        STATE_ATTR_CURRENT_TRANSIT: ephem.localtime(current_transit).isoformat(),
        STATE_ATTR_CURRENT_SETTING: ephem.localtime(current_setting).isoformat(),

        # Next dates data
        STATE_ATTR_NEXT_RISING: ephem.localtime(next_rising).isoformat(),
        STATE_ATTR_NEXT_TRANSIT: ephem.localtime(next_transit).isoformat(),
        STATE_ATTR_NEXT_SETTING: ephem.localtime(next_setting).isoformat(),
        STATE_ATTR_NEXT_NEW: ephem.localtime(next_new_moon).isoformat(),
        STATE_ATTR_NEXT_FIRST_QRT: ephem.localtime(next_first_quarter).isoformat(),
        STATE_ATTR_NEXT_FULL: ephem.localtime(next_full_moon).isoformat(),
        STATE_ATTR_NEXT_LAST_QRT: ephem.localtime(next_last_quarter).isoformat(),

        ATTR_LAST_UPDATED: ephem.localtime(g.date).isoformat(),
    }

    return data


def get_phase(lunation):
    """Return the state of the device."""
    if lunation < 0.5 or lunation > 27.5:
        return STATE_NEW_MOON
    if lunation < 6.5:
        return STATE_WAXING_CRESCENT
    if lunation < 7.5:
        return STATE_FIRST_QUARTER
    if lunation < 13.5:
        return STATE_WAXING_GIBBOUS
    if lunation < 14.5:
        return STATE_FULL_MOON
    if lunation < 20.5:
        return STATE_WANING_GIBBOUS
    if lunation < 21.5:
        return STATE_LAST_QUARTER
    return STATE_WANING_CRESCENT


def get_hms(hms_raw):
    """Return the elevation in format XXh YYm ZZ.ZZs"""
    hms = str(hms_raw).split(":")
    value = (
        hms[0].zfill(2) + "h " +
        hms[1].zfill(2) + "m " +
        hms[2].zfill(2) + "s"
    )

    return value
