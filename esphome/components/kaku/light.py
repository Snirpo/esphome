import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.kaku as kaku
from esphome.components import light
from esphome.const import CONF_OUTPUT_ID

kaku_ns = cg.esphome_ns.namespace('kaku')
KakuLightComponent = kaku_ns.class_('KakuLightComponent', light.LightOutput, cg.Component)

DEPENDENCIES = ['kaku']

CONF_ADDRESS = 'address'
CONF_UNIT = 'unit'

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(KakuLightComponent),
    cv.Required(CONF_ADDRESS): cv.uint32_t,
    cv.Required(CONF_UNIT): cv.uint8_t
}).extend(kaku.KAKU_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_unit(config[CONF_UNIT]))
    yield cg.register_component(var, config)
    yield light.register_light(var, config)
    yield kaku.register_kaku_device(var, config)
