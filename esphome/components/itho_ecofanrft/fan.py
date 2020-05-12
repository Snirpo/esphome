import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome import pins
from esphome.automation import maybe_simple_id
from esphome.components import cc1101, fan
from esphome.const import CONF_ID

DEPENDENCIES = ['cc1101']

itho_ecofanrft_ns = cg.esphome_ns.namespace('itho_ecofanrft')
IthoEcoFanRftComponent = itho_ecofanrft_ns.class_('IthoEcoFanRftComponent', cg.Component, fan.FanState)

# Actions
JoinAction = itho_ecofanrft_ns.class_('JoinAction', automation.Action)

AUTOLOAD = ['fan']

CONF_ITHO_IRQ_PIN = 'irq_pin'
CONF_RF_ADDRESS = 'rf_address'
CONF_PEER_RF_ADDRESS = 'peer_rf_address'


def validate(config):
    if CONF_PEER_RF_ADDRESS in config:
        if config[CONF_PEER_RF_ADDRESS] == config[CONF_RF_ADDRESS]:
            raise cv.Invalid("RF address cannot be the same as peer RF address!")

    return config


CONFIG_SCHEMA = cv.All(cv.Schema({
    cv.GenerateID(): cv.declare_id(IthoEcoFanRftComponent),
    cv.Required(CONF_ITHO_IRQ_PIN): pins.gpio_input_pin_schema,
    cv.Required(CONF_RF_ADDRESS): cv.rf_address,
    cv.Optional(CONF_PEER_RF_ADDRESS): cv.rf_address,
}).extend(cc1101.C1101_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA), validate)

ECOFAN_ACTION_SCHEMA = maybe_simple_id({
    cv.Required(CONF_ID): cv.use_id(IthoEcoFanRftComponent),
})


@automation.register_action('itho_ecofanrft.join', JoinAction, ECOFAN_ACTION_SCHEMA)
def fan_join_to_code(config, action_id, template_arg, args):
    parent = yield cg.get_variable(config[CONF_ID])
    yield cg.new_Pvariable(action_id, template_arg, parent)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    irq = yield cg.gpio_pin_expression(config[CONF_ITHO_IRQ_PIN])
    cg.add(var.set_irq_pin(irq))

    cg.add(var.set_rf_address(config[CONF_RF_ADDRESS].as_hex))
    if CONF_PEER_RF_ADDRESS in config:
        cg.add(var.set_peer_rf_address(config[CONF_PEER_RF_ADDRESS].as_hex))

    yield cc1101.register_c1101_device(var, config)
    yield fan.register_fan(var, config)
