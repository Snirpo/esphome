import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import remote_base, light
from esphome.const import CONF_ID

DEPENDENCIES = ['cc1101']

kaku_ns = cg.esphome_ns.namespace('kaku')
KakuComponent = kaku_ns.class_('KakuComponent', cg.Component)

AUTOLOAD = ['light']

CONF_RF_ADDRESS = 'rf_address'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KakuComponent),

}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    yield remote_base.register_listener(var, config)
    yield light.register_light(var, config)
