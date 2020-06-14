import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.kaku as kaku
from esphome.components import light
from esphome.const import CONF_OUTPUT_ID, CONF_ID

kaku_ns = cg.esphome_ns.namespace('kaku')
KakuLightComponent = kaku_ns.class_('KakuLightComponent', light.LightOutput, cg.Component)
KakuLightUnit = kaku_ns.class_('KakuLightUnit')

DEPENDENCIES = ['kaku']

CONF_ADDRESS = 'address'
CONF_UNIT = 'unit'
CONF_GROUP = 'group'
CONF_GROUPS = 'groups'
CONF_UNITS = 'units'

UNIT_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_ID): cv.declare_id(KakuLightUnit),
    cv.Required(CONF_ADDRESS): cv.uint32_t,
    cv.Required(CONF_UNIT): cv.uint8_t
})

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(KakuLightComponent),
    cv.Required(CONF_UNITS): cv.ensure_list(UNIT_SCHEMA),
    cv.Required(CONF_GROUPS): cv.ensure_list(cv.uint32_t)
}).extend(kaku.KAKU_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])

    for i, conf in enumerate(config[CONF_GROUPS]):
        cg.add(var.add_group(conf))

    for i, conf in enumerate(config[CONF_UNITS]):
        c = cg.variable(conf[CONF_ID], KakuLightUnit(conf[CONF_ADDRESS], conf[CONF_UNIT]))
        cg.add(var.add_unit(c))

    yield cg.register_component(var, config)
    yield light.register_light(var, config)
    yield kaku.register_kaku_device(var, config)
