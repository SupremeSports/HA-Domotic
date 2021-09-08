"""
This component provides support for a mqtt template switch.
"""

import logging
import voluptuous as vol

from homeassistant.helpers import config_validation as cv, template
from homeassistant.components.mqtt.switch import PLATFORM_SCHEMA, MqttSwitch
from homeassistant.const import (
    CONF_PAYLOAD_OFF,
    CONF_PAYLOAD_ON,
)
from homeassistant.components.mqtt import CONF_COMMAND_TOPIC

_LOGGER = logging.getLogger(__name__)


async def async_setup_platform(hass, config, async_add_entities, discovery_info=None):
    """Set up MQTT switch through configuration.yaml."""
    await _async_setup_entity(hass, config, async_add_entities, discovery_info)


async def _async_setup_entity(
    hass, config, async_add_entities, config_entry=None, discovery_data=None
):
    """Set up the MQTT switch."""
    async_add_entities([MqttTemplateSwitch(hass, config, config_entry, discovery_data)])


class MqttTemplateSwitch(MqttSwitch):
    """Representation of a switch that can be toggled using MQTT."""

    async def async_turn_on(self, **kwargs):
        """Turn the device on."""

        if isinstance(self._config[CONF_PAYLOAD_ON], str):
            self._config[CONF_PAYLOAD_ON] = self._get_template_data(self._config[CONF_PAYLOAD_ON])

        await MqttSwitch.async_turn_on(self, **kwargs)

    async def async_turn_off(self, **kwargs):
        """Turn the device off."""

        if isinstance(self._config[CONF_PAYLOAD_OFF], str):
            self._config[CONF_PAYLOAD_OFF] = self._get_template_data(self._config[CONF_PAYLOAD_OFF])

        await MqttSwitch.async_turn_off(self, **kwargs)

    def _get_template_data(self, payload_template: str) -> str:
        try:
            return template.Template(payload_template, self.hass).async_render()
        except template.jinja2.TemplateError as exc:
            _LOGGER.error(
                "Unable to publish to %s: rendering payload template of "
                "%s failed because %s",
                self._config[CONF_COMMAND_TOPIC],
                payload_template,
                exc,
            )
            return