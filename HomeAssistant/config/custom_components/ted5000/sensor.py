"""Support gathering ted5000 information."""
from __future__ import annotations

from datetime import datetime, timedelta, timezone
import logging

import requests
import voluptuous as vol
import xmltodict

from homeassistant.components.sensor import (
    PLATFORM_SCHEMA as PARENT_PLATFORM_SCHEMA,
    SensorDeviceClass,
    SensorEntity,
    SensorEntityDescription,
    SensorStateClass,
)
from homeassistant.const import (
    CONF_HOST,
    CONF_MODE,
    CONF_NAME,
    CONF_PORT,
    PERCENTAGE,
    UnitOfElectricPotential,
    UnitOfEnergy,
    UnitOfPower,
    UnitOfTime,
)
from homeassistant.core import HomeAssistant
from homeassistant.helpers import config_validation as cv
from homeassistant.helpers.entity_platform import AddEntitiesCallback
from homeassistant.helpers.typing import ConfigType, DiscoveryInfoType
from homeassistant.util import Throttle
from homeassistant.util.dt import as_local, get_time_zone

_LOGGER = logging.getLogger(__name__)

DOMAIN = "ted"

ENTITY_MTU_POWER = "power"
ENTITY_MTU_VOLTAGE = "voltage"
ENTITY_MTU_TODAY = "daily_energy"
ENTITY_MTU_MONTH = "monthly_energy"
ENTITY_MTU_PF = "pf"

ENTITY_MTUS = "MTUs"
ENTITY_RATE = "CurrentRate"
ENTITY_DAYSLEFT = "DaysLeftInBillingCycle"
ENTITY_PLANTYPE = "PlanType"
ENTITY_TIER = "CurrentTier"
ENTITY_TOU = "CurrentTOU"
ENTITY_TOUDESC = "CurrentTOUDescription"
ENTITY_CARBONRATE = "CarbonRate"
ENTITY_METERREAD = "MeterReadDate"
ENTITY_GTWY_DATETIME = "GatewayDateTime"

DEFAULT_NAME = DOMAIN

MIN_TIME_BETWEEN_UPDATES = timedelta(seconds=10)

SENSOR_TYPES_BASIC: tuple[SensorEntityDescription, ...] = (
    SensorEntityDescription(
        key=ENTITY_MTU_POWER,
        name="Power",
        device_class=SensorDeviceClass.POWER,
        state_class=SensorStateClass.MEASUREMENT,
        native_unit_of_measurement=UnitOfPower.WATT,
    ),
    SensorEntityDescription(
        key=ENTITY_MTU_VOLTAGE,
        name="Voltage",
        device_class=SensorDeviceClass.VOLTAGE,
        state_class=SensorStateClass.MEASUREMENT,
        native_unit_of_measurement=UnitOfElectricPotential.VOLT,
    ),
)

SENSOR_TYPES_ADVANCED: tuple[SensorEntityDescription, ...] = (
    SensorEntityDescription(
        key=ENTITY_MTU_TODAY,
        name="Energy Daily",
        device_class=SensorDeviceClass.ENERGY,
        state_class=SensorStateClass.TOTAL_INCREASING,
        native_unit_of_measurement=UnitOfEnergy.KILO_WATT_HOUR,
    ),
    SensorEntityDescription(
        key=ENTITY_MTU_MONTH,
        name="Energy Monthly",
        device_class=SensorDeviceClass.ENERGY,
        state_class=SensorStateClass.TOTAL_INCREASING,
        native_unit_of_measurement=UnitOfEnergy.KILO_WATT_HOUR,
    ),
    SensorEntityDescription(
        key=ENTITY_MTU_PF,
        name="Power Factor",
        device_class=SensorDeviceClass.POWER_FACTOR,
        state_class=SensorStateClass.MEASUREMENT,
        native_unit_of_measurement=PERCENTAGE,
    ),
)

SENSOR_TYPES_EXTENDED: tuple[SensorEntityDescription, ...] = (
    # MTUs Quantity
    SensorEntityDescription(
        key=ENTITY_MTUS,
        name="MTU Quantity",
    ),
    # Current Rate $/kWh
    SensorEntityDescription(
        key=ENTITY_RATE,
        name="Current Rate",
        device_class=SensorDeviceClass.MONETARY,
        #state_class=SensorStateClass.MEASUREMENT,
        native_unit_of_measurement="$/kWh",
    ),
    # Days left in billing cycle
    SensorEntityDescription(
        key=ENTITY_DAYSLEFT,
        name="Days Left Billing Cycle",
        native_unit_of_measurement=UnitOfTime.DAYS,
    ),
    # Plan type (Flat, Tier, TOU, Tier+TOU)
    SensorEntityDescription(
        key=ENTITY_PLANTYPE,
        name="Plan Type",
    ),
    # Current Tier (0 = Disabled)
    SensorEntityDescription(
        key=ENTITY_TIER,
        name="Current Tier",
    ),
    # Current TOU (0 = Disabled)
    SensorEntityDescription(key=ENTITY_TOU, name="Current TOU"),
    # Current TOU Description (if Current TOU is 0 => Not Configured)
    SensorEntityDescription(
        key=ENTITY_TOUDESC,
        name="Current TOU Description",
    ),
    # Carbon Rate lbs/kW
    SensorEntityDescription(
        key=ENTITY_CARBONRATE,
        name="Carbon Rate",
        device_class=SensorDeviceClass.MONETARY,
        #state_class=SensorStateClass.MEASUREMENT,
        native_unit_of_measurement="lbs/kW",
    ),
    # Meter read date
    SensorEntityDescription(
        key=ENTITY_METERREAD,
        name="Meter Read Date",
    ),
    # Gateway date/time
    SensorEntityDescription(
        key=ENTITY_GTWY_DATETIME,
        name="Gateway Date and Time",
        device_class=SensorDeviceClass.TIMESTAMP,
    ),
)

PLATFORM_SCHEMA = PARENT_PLATFORM_SCHEMA.extend(
    {
        vol.Required(CONF_HOST): cv.string,
        vol.Optional(CONF_PORT, default=80): cv.port,
        vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string,
        vol.Optional(CONF_MODE, default="base"): cv.string,
    }
)


def setup_platform(
    hass: HomeAssistant,
    config: ConfigType,
    add_entities: AddEntitiesCallback,
    discovery_info: DiscoveryInfoType | None = None,
) -> None:
    """Set up the ted5000 sensor platform."""
    host = config.get(CONF_HOST)
    port = config.get(CONF_PORT)
    name = config.get(CONF_NAME)
    mode = config.get(CONF_MODE)
    url = f"http://{host}:{port}/api/LiveData.xml"
    
    time_zone = hass.config.time_zone

    gateway = Ted5000Data(url, time_zone)
    # Get MTU information to create the sensors.
    gateway.update()

    entities = []

    # Create MTU sensors
    for mtu in range(1, gateway.mtus + 1):
        for description in SENSOR_TYPES_BASIC:
            entities.append(Ted5000SensorEntity(gateway, mtu, description, name))
        if mode in {"advanced", "extended"}:  # advanced or extended
            for description in SENSOR_TYPES_ADVANCED:
                entities.append(Ted5000SensorEntity(gateway, mtu, description, name))

    # Create utility sensors
    if mode == "extended":  # extended only
        for description in SENSOR_TYPES_EXTENDED:
            entities.append(Ted5000SensorEntity(gateway, 0, description, name))

    add_entities(entities, True)


def get_ted5000(self) -> int | None:
    """Collect data from the ted5000 gateway."""

    url = self.url
    time_zone = self.tz
    self.data = {}
    self.data_utility = {}

    try:
        request = requests.get(url, timeout=10)
    except requests.exceptions.RequestException as err:
        _LOGGER.error("No connection to endpoint: %s", err)
    else:
        doc = xmltodict.parse(request.text)
        mtus = int(doc["LiveData"]["System"]["NumberMTU"])

        # MTU Data
        for mtu in range(1, mtus + 1):
            power = int(doc["LiveData"]["Power"]["MTU%d" % mtu]["PowerNow"])
            voltage = int(doc["LiveData"]["Voltage"]["MTU%d" % mtu]["VoltageNow"])
            energy_tdy = int(doc["LiveData"]["Power"]["MTU%d" % mtu]["PowerTDY"])
            energy_mtd = int(doc["LiveData"]["Power"]["MTU%d" % mtu]["PowerMTD"])
            power_factor = int(doc["LiveData"]["Power"]["MTU%d" % mtu]["PF"])

            self.data[mtu] = {
                ENTITY_MTU_POWER: power,
                ENTITY_MTU_VOLTAGE: voltage / 10,
                ENTITY_MTU_TODAY: energy_tdy / 1000,
                ENTITY_MTU_MONTH: energy_mtd / 1000,
                ENTITY_MTU_PF: power_factor / 10,
            }

        # Utility Data
        current_rate = int(doc["LiveData"]["Utility"]["CurrentRate"])
        days_left = int(doc["LiveData"]["Utility"]["DaysLeftInBillingCycle"])
        plan_type = int(doc["LiveData"]["Utility"]["PlanType"])
        plan_type_str = {0: "Flat", 1: "Tier", 2: "TOU", 3: "Tier+TOU"}
        carbon_rate = int(doc["LiveData"]["Utility"]["CarbonRate"])
        read_date = int(doc["LiveData"]["Utility"]["MeterReadDate"])
        gateway_year = int(doc["LiveData"]["GatewayTime"]["Year"]) + 2000
        gateway_month = int(doc["LiveData"]["GatewayTime"]["Month"])
        gateway_day = int(doc["LiveData"]["GatewayTime"]["Day"])
        gateway_hour = int(doc["LiveData"]["GatewayTime"]["Hour"])
        gateway_minute = int(doc["LiveData"]["GatewayTime"]["Minute"])
        gateway_second = int(doc["LiveData"]["GatewayTime"]["Second"])

        try:
            gateway_datetime = datetime(gateway_year, gateway_month, gateway_day, gateway_hour, gateway_minute, gateway_second)
        except ValueError:
            gateway_datetime = datetime(1970, 1, 1, 0, 0, 0)

        gateway_utc_time = gateway_datetime.replace(tzinfo=get_time_zone(time_zone))

        if plan_type in (0, 2):
            current_tier = 0
        else:
            current_tier = int(doc["LiveData"]["Utility"]["CurrentTier"]) + 1

        if plan_type < 2:
            current_tou = 0
            current_tou_str = "Not Configured"
        else:
            current_tou = int(doc["LiveData"]["Utility"]["CurrentTOU"]) + 1
            current_tou_str = doc["LiveData"]["Utility"]["CurrentTOUDescription"]

        self.data_utility = {
            ENTITY_MTUS: mtus,
            ENTITY_RATE: current_rate / 100000,
            ENTITY_DAYSLEFT: days_left,
            ENTITY_PLANTYPE: plan_type_str[plan_type],
            ENTITY_TIER: current_tier,
            ENTITY_TOU: current_tou,
            ENTITY_TOUDESC: current_tou_str,
            ENTITY_CARBONRATE: carbon_rate / 100,
            ENTITY_METERREAD: read_date,
            ENTITY_GTWY_DATETIME: gateway_utc_time,
        }

        return mtus

    return 0


class Ted5000Data:
    """Collect data from the ted5000 gateway."""

    def __init__(self, url, tz):
        """Initialize the data object."""
        self.url = url
        self.tz = tz
        self.mtus = 0

    @Throttle(MIN_TIME_BETWEEN_UPDATES)
    def update(self):
        """Get the latest data from ted5000."""
        # Update data
        self.mtus = get_ted5000(self)


class Ted5000SensorEntity(SensorEntity):
    """Representation of the current ted5000 data."""

    _attr_has_entity_name = True

    def __init__(
        self,
        gateway,
        mtu,
        description: SensorEntityDescription,
        name,
    ):
        """Initialize the sensor."""
        self.entity_description = description
        if mtu > 0:
            self._attr_name = f"{name} MTU{mtu} {description.name}"
        else:
            self._attr_name = f"{name} Utility {description.name}"
        self.gateway = gateway
        self._mtu = mtu

    def update(self):
        """Get the latest data from Ted5000 and update the state."""
        self.gateway.update()
        if self._mtu > 0:
            if self.entity_description.key in self.gateway.data[self._mtu]:
                self._attr_native_value = self.gateway.data[self._mtu][
                    self.entity_description.key
                ]
        if self.entity_description.key in self.gateway.data_utility:
            self._attr_native_value = self.gateway.data_utility[
                self.entity_description.key
            ]
