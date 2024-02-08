"""Camera that loads a picture from an API link specified by a MQTT topic."""
from __future__ import annotations
import logging

import httpx
import voluptuous as vol
from urllib.parse import urljoin

from homeassistant.components import mqtt
from homeassistant.components.mqtt.const import CONF_QOS, CONF_STATE_TOPIC as CONF_TOPIC
from homeassistant.components.camera import Camera, PLATFORM_SCHEMA as PARENT_PLATFORM_SCHEMA

from homeassistant.const import CONF_NAME, CONF_HOST

from homeassistant.exceptions import TemplateError
from homeassistant.helpers import config_validation as cv
from homeassistant.helpers.httpx_client import get_async_client
from homeassistant.helpers.reload import async_setup_reload_service

from . import DOMAIN, PLATFORMS

_LOGGER = logging.getLogger(__name__)
CONF_CONTENT_TYPE = "content_type"
CONF_FRAMERATE = "framerate"

DEFAULT_NAME = "MQTT API Camera"
GET_IMAGE_TIMEOUT = 10

PLATFORM_SCHEMA = PARENT_PLATFORM_SCHEMA.extend(
    {
        vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string,
        vol.Required(CONF_HOST): cv.string,
        vol.Optional(CONF_FRAMERATE, default=2): vol.Any(
            cv.small_float, cv.positive_int
        ),
    }
).extend(mqtt.config.MQTT_RO_SCHEMA.schema)


async def async_setup_platform(hass, config, async_add_entities, discovery_info=None):
    """Set up a generic IP Camera."""

    await async_setup_reload_service(hass, DOMAIN, PLATFORMS)

    async_add_entities([MqttAPICamera(hass, config)])


class MqttAPICamera(Camera):
    """representation of a MQTT camera."""

    def __init__(self, hass, device_info):
        """Initialize the MQTT Camera."""

        super().__init__()
        self.hass = hass
        self._config = device_info
        self._topic = device_info.get(CONF_TOPIC)
        self._qos = device_info.get(CONF_QOS)
        self._host = device_info.get(CONF_HOST)
        self._frame_interval = device_info[CONF_FRAMERATE]
        self._supported_features = 0
        self._auth = None

        name = device_info.get(CONF_NAME)
        if name:
            self._attr_name = name
        self._attr_unique_id = "mqttapi-{}_{}".format(self._host, self._topic)

        self._auth = None

        self._still_image_url = None
        self._last_image = None
        self._last_url = None

    async def async_added_to_hass(self):
        """Subscribe to MQTT events."""

        def message_received(msg):
            """Handle new MQTT messages."""
            self._still_image_url = urljoin(self._host, msg.payload.replace('"', ""))
            _LOGGER.info("Updating still image url to '%s'", self._still_image_url)

        await mqtt.async_subscribe(
            self.hass,
            self._topic,
            message_received,
            self._qos,
        )

    @property
    def supported_features(self):
        """Return supported features for this camera."""
        return self._supported_features

    @property
    def frame_interval(self):
        """Return the interval between frames of the mjpeg stream."""
        return self._frame_interval

    async def async_camera_image(
        self, width: int | None = None, height: int | None = None
    ) -> bytes | None:
        """Return a still image response from the camera."""
        try:
            url = self._still_image_url
        except TemplateError as err:
            _LOGGER.error("Error parsing template %s: %s", self._still_image_url, err)
            return self._last_image

        try:
            async_client = get_async_client(self.hass)
            response = await async_client.get(
                url, auth=self._auth, timeout=GET_IMAGE_TIMEOUT
            )
            response.raise_for_status()
            self._last_image = response.content
        except httpx.TimeoutException:
            _LOGGER.error("Timeout getting camera image from %s", self._attr_name)
            return self._last_image
        except (httpx.RequestError, httpx.HTTPStatusError) as err:
            _LOGGER.error("Error getting new camera image from %s: %s", self._attr_name, err)
            return self._last_image

        self._last_url = url
        return self._last_image

