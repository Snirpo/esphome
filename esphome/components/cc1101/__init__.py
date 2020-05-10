import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome.const import CONF_ID

DEPENDENCIES = ['spi']
MULTI_CONF = False

cc1101_ns = cg.esphome_ns.namespace('cc1101')
CC1101Component = cc1101_ns.class_('CC1101Component', cg.Component, spi.SPIDevice)

CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID(): cv.declare_id(CC1101Component),
}).extend(cv.COMPONENT_SCHEMA).extend(spi.SPI_DEVICE_SCHEMA))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
