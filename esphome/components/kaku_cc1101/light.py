import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cc1101, light
from esphome.const import CONF_ID

DEPENDENCIES = ['cc1101']

kaku_cc1101_ns = cg.esphome_ns.namespace('kaku_cc1101')
KakuCC1101Component = kaku_cc1101_ns.class_('KakuCC1101Component', cg.Component)

AUTOLOAD = ['light']

CONF_RF_ADDRESS = 'rf_address'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KakuCC1101Component),
    cv.Required(CONF_RF_ADDRESS): cv.rf_address
}).extend(cc1101.C1101_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    cg.add(var.set_rf_address(config[CONF_RF_ADDRESS].as_hex))
    yield cc1101.register_c1101_device(var, config)
    yield light.register_light(var, config)
