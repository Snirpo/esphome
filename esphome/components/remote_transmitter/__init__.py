import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import remote_base, cc1101
from esphome.const import CONF_CARRIER_DUTY_PERCENT, CONF_ID, CONF_PIN

AUTO_LOAD = ['remote_base']
remote_transmitter_ns = cg.esphome_ns.namespace('remote_transmitter')
RemoteTransmitterComponent = remote_transmitter_ns.class_('RemoteTransmitterComponent',
                                                          remote_base.RemoteTransmitterBase,
                                                          cc1101.CC1101Device,
                                                          cg.Component)

MULTI_CONF = True

CONF_USE_CC1101 = "use_cc1101"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(RemoteTransmitterComponent),
    cv.Required(CONF_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_USE_CC1101, default=False): cv.boolean,
    cv.Required(CONF_CARRIER_DUTY_PERCENT): cv.All(cv.percentage_int, cv.Range(min=1, max=100)),
}).extend(cc1101.C1101_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    pin = yield cg.gpio_pin_expression(config[CONF_PIN])
    var = cg.new_Pvariable(config[CONF_ID], pin)

    if config[CONF_USE_CC1101]:
        yield cc1101.register_c1101_device(var, config)

    yield cg.register_component(var, config)

    cg.add(var.set_carrier_duty_percent(config[CONF_CARRIER_DUTY_PERCENT]))
