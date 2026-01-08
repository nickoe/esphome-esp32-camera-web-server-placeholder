import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MODE, CONF_PORT
from esphome.types import ConfigType

CODEOWNERS = ["@nickoe"]
AUTO_LOAD = ["camera"]
DEPENDENCIES = ["network"]
MULTI_CONF = True

esp32_camera_web_server_placeholder_ns = cg.esphome_ns.namespace("esp32_camera_web_server_placeholder")
CameraWebServerPlaceholder = esp32_camera_web_server_placeholder_ns.class_(
    "CameraWebServerPlaceholder", cg.Component
)
Mode = esp32_camera_web_server_placeholder_ns.enum("Mode")

MODES = {"STREAM": Mode.STREAM, "SNAPSHOT": Mode.SNAPSHOT}

CONF_PLACEHOLDER_ENABLED = "placeholder_enabled"

def _consume_sockets(config: ConfigType) -> ConfigType:
    from esphome.components import socket
    socket.consume_sockets(3, "esp32_camera_web_server_placeholder")(config)
    return config

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(CameraWebServerPlaceholder),
            cv.Required(CONF_PORT): cv.port,
            cv.Required(CONF_MODE): cv.enum(MODES, upper=True),
            cv.Optional(CONF_PLACEHOLDER_ENABLED, default=True): cv.boolean,
        },
    ).extend(cv.COMPONENT_SCHEMA),
    _consume_sockets,
)

async def to_code(config):
    server = cg.new_Pvariable(config[CONF_ID])
    cg.add(server.set_port(config[CONF_PORT]))
    cg.add(server.set_mode(config[CONF_MODE]))
    cg.add(server.set_placeholder_enabled(config[CONF_PLACEHOLDER_ENABLED]))
    await cg.register_component(server, config)
