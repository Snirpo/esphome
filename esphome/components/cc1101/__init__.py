import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import spi
from esphome.const import CONF_ID, CONF_CC1101_ID
from esphome.core import coroutine

DEPENDENCIES = ['spi']

cc1101_ns = cg.esphome_ns.namespace('cc1101')
CC1101Component = cc1101_ns.class_('CC1101Component', cg.Component, spi.SPIDevice)

CONF_IRQ_PIN = 'irq_pin'
CONF_SERIAL = 'serial'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CC1101Component),
    cv.Optional(CONF_IRQ_PIN): pins.gpio_input_pin_schema,
    cv.Optional(CONF_SERIAL): cv.boolean
}).extend(cv.COMPONENT_SCHEMA).extend(spi.SPI_DEVICE_SCHEMA)

C1101_DEVICE_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_CC1101_ID): cv.use_id(CC1101Component)
})


@coroutine
def register_c1101_device(var, config):
    parent = yield cg.get_variable(config[CONF_CC1101_ID])
    cg.add(var.set_cc1101_parent(parent))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    if CONF_SERIAL in config:
        cg.add(var.set_serial_mode(config[CONF_SERIAL]))

    if CONF_IRQ_PIN in config:
        irq_pin = yield cg.gpio_pin_expression(config[CONF_IRQ_PIN])
        cg.add(var.set_irq_pin(irq_pin))

    yield cg.register_component(var, config)
    yield spi.register_spi_device(var, config)
