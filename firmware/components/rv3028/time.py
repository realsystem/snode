import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, time as time_
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]

rv3028_ns = cg.esphome_ns.namespace("rv3028")
RV3028Component = rv3028_ns.class_(
    "RV3028Component", time_.RealTimeClock, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    time_.TIME_SCHEMA.extend({cv.GenerateID(): cv.declare_id(RV3028Component)})
    .extend(i2c.i2c_device_schema(0x52))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    await time_.register_time(var, config)
