"""Component to make instant statistics about your history."""
from __future__ import annotations

from abc import abstractmethod
import datetime

import voluptuous as vol

from homeassistant.components.sensor import (
    PLATFORM_SCHEMA,
    SensorDeviceClass,
    SensorEntity,
    SensorStateClass,
)
from homeassistant.const import (
    CONF_ENTITY_ID,
    CONF_NAME,
    CONF_STATE,
    CONF_TYPE,
    PERCENTAGE,
    UnitOfTime,
)
from homeassistant.core import HomeAssistant, callback
from homeassistant.exceptions import PlatformNotReady
import homeassistant.helpers.config_validation as cv
from homeassistant.helpers.entity_platform import AddEntitiesCallback
from homeassistant.helpers.reload import async_setup_reload_service
from homeassistant.helpers.template import Template
from homeassistant.helpers.typing import ConfigType, DiscoveryInfoType
from homeassistant.helpers.update_coordinator import CoordinatorEntity

from . import DOMAIN, PLATFORMS
from .coordinator import HistoryStatsUpdateCoordinator
from .data import HistoryStats
from .helpers import pretty_ratio

CONF_START = "start"
CONF_END = "end"
CONF_DURATION = "duration"
CONF_PERIOD_KEYS = [CONF_START, CONF_END, CONF_DURATION]

CONF_TYPE_TIME = "time"
CONF_TYPE_RATIO = "ratio"
CONF_TYPE_COUNT = "count"
CONF_TYPE_KEYS = [CONF_TYPE_TIME, CONF_TYPE_RATIO, CONF_TYPE_COUNT]

DEFAULT_NAME = "unnamed statistics"
UNITS: dict[str, str] = {
    CONF_TYPE_TIME: UnitOfTime.SECONDS,
    CONF_TYPE_RATIO: PERCENTAGE,
    CONF_TYPE_COUNT: "",
}
ICON = "mdi:chart-line"


def exactly_two_period_keys(conf):
    """Ensure exactly 2 of CONF_PERIOD_KEYS are provided."""
    if sum(param in conf for param in CONF_PERIOD_KEYS) != 2:
        raise vol.Invalid(
            "You must provide exactly 2 of the following: start, end, duration"
        )
    return conf


PLATFORM_SCHEMA = vol.All(
    PLATFORM_SCHEMA.extend(
        {
            vol.Required(CONF_ENTITY_ID): cv.entity_id,
            vol.Required(CONF_STATE): vol.All(cv.ensure_list, [cv.string]),
            vol.Optional(CONF_START): cv.template,
            vol.Optional(CONF_END): cv.template,
            vol.Optional(CONF_DURATION): cv.time_period,
            vol.Optional(CONF_TYPE, default=CONF_TYPE_TIME): vol.In(CONF_TYPE_KEYS),
            vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string,
        }
    ),
    exactly_two_period_keys,
)


# noinspection PyUnusedLocal
async def async_setup_platform(
    hass: HomeAssistant,
    config: ConfigType,
    async_add_entities: AddEntitiesCallback,
    discovery_info: DiscoveryInfoType | None = None,
) -> None:
    """Set up the History Stats sensor."""
    await async_setup_reload_service(hass, DOMAIN, PLATFORMS)

    entity_id: str = config[CONF_ENTITY_ID]
    entity_states: list[str] = config[CONF_STATE]
    start: Template | None = config.get(CONF_START)
    end: Template | None = config.get(CONF_END)
    duration: datetime.timedelta | None = config.get(CONF_DURATION)
    sensor_type: str = config[CONF_TYPE]
    name: str = config[CONF_NAME]

    for template in (start, end):
        if template is not None:
            template.hass = hass

    history_stats = HistoryStats(hass, entity_id, entity_states, start, end, duration)
    coordinator = HistoryStatsUpdateCoordinator(hass, history_stats, name)
    await coordinator.async_refresh()
    if not coordinator.last_update_success:
        raise PlatformNotReady from coordinator.last_exception
    async_add_entities([HistoryStatsSensor(coordinator, sensor_type, name)])


class HistoryStatsSensorBase(
    CoordinatorEntity[HistoryStatsUpdateCoordinator], SensorEntity
):
    """Base class for a HistoryStats sensor."""

    _attr_icon = ICON

    def __init__(
        self,
        coordinator: HistoryStatsUpdateCoordinator,
        name: str,
    ) -> None:
        """Initialize the HistoryStats sensor base class."""
        super().__init__(coordinator)
        self._attr_name = name

    async def async_added_to_hass(self) -> None:
        """Entity has been added to hass."""
        await super().async_added_to_hass()
        self.async_on_remove(self.coordinator.async_setup_state_listener())

    def _handle_coordinator_update(self) -> None:
        """Set attrs from value and count."""
        self._process_update()
        super()._handle_coordinator_update()

    @callback
    @abstractmethod
    def _process_update(self) -> None:
        """Process an update from the coordinator."""


class HistoryStatsSensor(HistoryStatsSensorBase):
    """A HistoryStats sensor."""

    _attr_state_class = SensorStateClass.MEASUREMENT

    def __init__(
        self,
        coordinator: HistoryStatsUpdateCoordinator,
        sensor_type: str,
        name: str,
    ) -> None:
        """Initialize the HistoryStats sensor."""
        super().__init__(coordinator, name)
        self._attr_native_unit_of_measurement = UNITS[sensor_type]
        self._type = sensor_type
        self._process_update()
        if self._type == CONF_TYPE_TIME:
            self._attr_device_class = SensorDeviceClass.DURATION

    @callback
    def _process_update(self) -> None:
        """Process an update from the coordinator."""
        state = self.coordinator.data
        if state is None or state.seconds_matched is None:
            self._attr_native_value = None
            return

        if self._type == CONF_TYPE_TIME:
            self._attr_native_value = int(state.seconds_matched)
        elif self._type == CONF_TYPE_RATIO:
            self._attr_native_value = pretty_ratio(state.seconds_matched/3600, state.period)
        elif self._type == CONF_TYPE_COUNT:
            self._attr_native_value = state.match_count