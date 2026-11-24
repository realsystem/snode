# SNODE powered-bus (O2) master for ESPHome. Protocol: docs/10-bus-protocol.md. Design log D35, D38-D40.
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

CODEOWNERS = []
DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_SLOTS = "slots"
CONF_REPLY_TIMEOUT = "reply_timeout"
CONF_RETRIES = "retries"

snode_bus_ns = cg.esphome_ns.namespace("snode_bus")
SnodeBus = snode_bus_ns.class_("SnodeBus", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SnodeBus),
            # Probe serials in slot order (slot 1 first); 0 = empty slot. Max 10 per bus.
            cv.Optional(CONF_SLOTS, default=[]): cv.All(
                cv.ensure_list(cv.hex_uint32_t), cv.Length(max=10)
            ),
            cv.Optional(CONF_REPLY_TIMEOUT, default="25ms"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_RETRIES, default=2): cv.int_range(min=0, max=5),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    for s in config[CONF_SLOTS]:
        cg.add(var.add_slot(s))
    cg.add(var.set_reply_timeout(config[CONF_REPLY_TIMEOUT]))
    cg.add(var.set_retries(config[CONF_RETRIES]))
