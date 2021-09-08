"""Time Counter"""

"""

Configuration reference:

    time_counter:
      my_counter:
        name: string
        duration: int, required [s]
        initial: int, defaults to 0
        min: int, defaults to 0
        max: int, defaults to 100
        debounce: int, optional, defaults to 1000 [ms]
        
    You can also set icon and restore as documented in counter component.

Available services are:

    time_counter.upcount – starts counting towards MAX.
    time_counter.downcount – starts counting towards MIN.
    time_counter.set – starts counting towards state: int value given in data for service.
    time_counter.stop – stops counting immediately.

There are two events:

    time_counter.started – fired when counter starts counting.
    time_counter.stopped – fired when counter stopped counting.
    This is:
    a) When timer reached boundaries (MIN or MAX).
    b) When timer reached state given in time_counter.set service call.
    You can also get state of counter via event.state.

How it works:

    The idea of component is this: we start at INITIAL and store current state between MIN and MAX as entity state. Using downcount or upcount we start changing current state towards MIN or MAX in given part of time DURATION to reflect real device behind this timer.

    You can use this component to control devices that cannot provide state informations, but you know the time between MIN and MAX, so you can change states on your own.

    An example can be two 230V switch covers: switch-1 opens the cover, switch-2 closes the cover. We don’t know the moment in which cover is in particular state, but we know the time. So we can use it now.

"""

"""Time-based counter."""
from datetime import timedelta
import logging

import voluptuous as vol

from homeassistant.const import ATTR_ENTITY_ID, CONF_ICON, CONF_NAME
import homeassistant.helpers.config_validation as cv
from homeassistant.helpers.entity_component import EntityComponent
from homeassistant.helpers.event import async_track_point_in_utc_time, async_track_time_interval
from homeassistant.helpers.restore_state import RestoreEntity
import homeassistant.util.dt as dt_util

_LOGGER = logging.getLogger(__name__)

DOMAIN = 'time_counter'
ENTITY_ID_FORMAT = DOMAIN + '.{}'

ATTR_STATE = 'state'

CONF_RESTORE = 'restore'

ATTR_DURATION = 'duration'
CONF_DURATION = 'duration'

DEFAULT_INITIAL = 0
ATTR_INITIAL = 'initial'
CONF_INITIAL = 'initial'

DEFAULT_MIN = 0
ATTR_MIN = 'min'
CONF_MIN = 'min'

DEFAULT_MAX = 100
ATTR_MAX = 'max'
CONF_MAX = 'max'

DEFAULT_DEBOUNCE = 1000
ATTR_DEBOUNCE = 'debounce'
CONF_DEBOUNCE = 'debounce'

MODE_UPCOUNTING = 1
MODE_DOWNCOUNTING = -1

EVENT_TIME_COUNTER_STARTED = 'time_counter.started'
EVENT_TIME_COUNTER_STOPPED = 'time_counter.stopped'

SERVICE_UPCOUNT = 'upcount'
SERVICE_DOWNCOUNT = 'downcount'
SERVICE_STOP = 'stop'
SERVICE_SET = 'set'

SERVICE_SCHEMA = vol.Schema({
    vol.Required(ATTR_ENTITY_ID): cv.comp_entity_ids,
})

SERVICE_SCHEMA_SET = vol.Schema({
    vol.Required(ATTR_ENTITY_ID): cv.comp_entity_ids,
    vol.Required(ATTR_STATE): cv.positive_int,
})

CONFIG_SCHEMA = vol.Schema({
    DOMAIN: cv.schema_with_slug_keys(
        vol.Any({
            vol.Optional(CONF_NAME): cv.string,
            vol.Optional(CONF_ICON): cv.icon,
            vol.Optional(CONF_RESTORE, default=True): cv.boolean,
            vol.Required(CONF_DURATION):
                cv.positive_int,
            vol.Optional(CONF_INITIAL, default=DEFAULT_INITIAL):
                cv.positive_int,
            vol.Optional(CONF_MIN, default=DEFAULT_MIN):
                cv.positive_int,
            vol.Optional(CONF_MAX, default=DEFAULT_MAX):
                cv.positive_int,
            vol.Optional(CONF_DEBOUNCE, default=DEFAULT_DEBOUNCE):
                cv.positive_int,
        }, None)
    )
}, extra=vol.ALLOW_EXTRA)

async def async_setup(hass, config):
    """Set up a timer."""
    component = EntityComponent(_LOGGER, DOMAIN, hass)

    entities = []

    for object_id, cfg in config[DOMAIN].items():
        if not cfg:
            cfg = {}

        name = cfg.get(CONF_NAME)
        icon = cfg.get(CONF_ICON)
        restore = cfg.get(CONF_RESTORE)
        duration = cfg.get(CONF_DURATION)
        initial = cfg.get(CONF_INITIAL)
        min = cfg.get(CONF_MIN)
        max = cfg.get(CONF_MAX)
        debounce = cfg.get(CONF_DEBOUNCE)

        entities.append(TimeCounter(hass, object_id, name, icon, restore, duration, initial, min, max, debounce))

    if not entities:
        return False

    component.async_register_entity_service(
        SERVICE_UPCOUNT, SERVICE_SCHEMA,
        'async_upcount')
    component.async_register_entity_service(
        SERVICE_DOWNCOUNT, SERVICE_SCHEMA,
        'async_downcount')
    component.async_register_entity_service(
        SERVICE_STOP, SERVICE_SCHEMA,
        'async_stop')
    component.async_register_entity_service(
        SERVICE_SET, SERVICE_SCHEMA_SET,
        'async_set')

    await component.async_add_entities(entities)
    return True


class TimeCounter(RestoreEntity):
    """Representation of a timer."""

################ Constructor ################

    def __init__(self, hass, object_id, name, icon, restore, duration, initial, min, max, debounce):
        """Initialize a timer."""
        self._hass = hass

        self.entity_id = ENTITY_ID_FORMAT.format(object_id)
        self._name = name
        self._icon = icon
        self._restore = restore
        self._duration = duration
        self._min = min
        self._max = max

        self._state = self._initial = initial

        self._debounce = debounce
        self._is_locked = False

        self._debounce_listener = None # fired when debunce ends
        self._debounce_end = None # endtime of debounce

        self._mode = None # 1 for upcount or -1 for downcount
        self._target = None # max/min for upcount/downcount, target if set

        self._boundary_listener = None # fired when min or max reach
        self._boundary_end = None # endtime of reaching min or max

################ Hass methods ################

    @property
    def should_poll(self):
        """If entity should be polled."""
        return False

    @property
    def name(self):
        """Return name of the timer."""
        return self._name

    @property
    def icon(self):
        """Return the icon to be used for this entity."""
        return self._icon

    @property
    def state(self):
        """Return the current value of the timer."""
        return self._state

    @property
    def state_attributes(self):
        """Return the state attributes."""
        return {
            ATTR_DURATION: self._duration,
            ATTR_INITIAL: self._initial,
            ATTR_MIN: self._min,
            ATTR_MAX: self._max,
            ATTR_DEBOUNCE: self._debounce
        }

    async def async_added_to_hass(self):
        """Call when entity about to be added to Home Assistant."""
        await super().async_added_to_hass()
        # __init__ will set self._state to self._initial, only override
        # if needed.
        if self._restore:
            state = await self.async_get_last_state()
            if state is not None:
                self._state = float(state.state)

################ Component methods ################

    async def async_upcount(self):
        """Start upcounting."""
        _LOGGER.info("Start upcount...")
        await self.async_set(self._max)

    async def async_downcount(self):
        """Start downcounting."""
        _LOGGER.info("Start downcount...")
        await self.async_set(self._min)

    async def async_set(self, state):
        """Set given state value by running time counter for calculated amount of time."""
        if self._is_locked:
            _LOGGER.info("Timer is locked by debounce.")
            return

        self._debounce_end = dt_util.utcnow() + timedelta(seconds=(self._debounce / 1000))
        self._debounce_listener = async_track_point_in_utc_time(self._hass,
                                                       self.async_unlock_debounce,
                                                       self._debounce_end)
        self._is_locked = True

        _LOGGER.info("Setting value:")

        if state > self._max:
            state = self._max
        elif state < self._min:
            state = self._min

        _LOGGER.info(state)

        # in case other time counter is running
        await self.async_stop()

        self._target = state

        if self._state < state:
            # We will upcount.
            self._mode = MODE_UPCOUNTING
            upcount_value = state - self._state
            count_time = self._duration * upcount_value / self._max
        else:
            # We will downcount.
            self._mode = MODE_DOWNCOUNTING
            downcount_value = self._state - state
            count_time = self._duration * downcount_value / self._max

        self._boundary_end = dt_util.utcnow() + timedelta(seconds=count_time) # timedelta.seconds can be float! using it!
        _LOGGER.info("Shall end at:")
        _LOGGER.info(self._boundary_end)

        self._hass.bus.async_fire(EVENT_TIME_COUNTER_STARTED,
                                  {"entity_id": self.entity_id})

        self._boundary_listener = async_track_point_in_utc_time(self._hass,
                                                       self.async_finish,
                                                       self._boundary_end)

    async def async_stop(self):
        """Stop counter."""
        _LOGGER.info("Timer stop.")

        if self._boundary_listener:
            self._boundary_listener()
            self._boundary_listener = None

        when = dt_util.utcnow()
        await self.async_update_state(when)

################ Listener actions ################

    async def async_finish(self, time):
        """Timer stopped by reaching boundary or manual trigger."""
        _LOGGER.info("Timer finished.")

        self._boundary_listener = None

        await self.async_update_state(time)

    async def async_update_state(self, time):
        """Update the state."""
        _LOGGER.info("Updating state:")

        if self._boundary_end is None or self._mode is None or self._target is None:
            _LOGGER.info("No previously running time counter.")
            return

        time_diff = self._boundary_end - time
        time_to = time_diff.total_seconds() # timedelta.seconds can be float! using it!

        if time_to < 0:
            time_to = 0

        if self._mode > 0:
            # Upcounting.
            self._state = self._target - (time_to * self._max / self._duration)
        else:
            # Downcounting.
            self._state = self._target + (time_to * self._max / self._duration)

        _LOGGER.info(self._state)

        self._target = None
        self._mode = None
        self._boundary_end = None

        self._hass.bus.async_fire(EVENT_TIME_COUNTER_STOPPED,
                                  {"entity_id": self.entity_id, "state": self._state})

        await self.async_update_ha_state()

    async def async_unlock_debounce(self, time):
        """Unlocking debounce."""
        _LOGGER.info("Debounce unlocked.")
        self._is_locked = False
        self._debounce_listener = None
