import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.somfy as somfy
from esphome.components import cover
from esphome.const import CONF_ID

somfy_ns = cg.esphome_ns.namespace('somfy')
SomfyCoverComponent = somfy_ns.class_('SomfyCoverComponent', cover.Cover, cg.Component)

DEPENDENCIES = ['somfy']

CONF_ADDRESS = 'address'

CONFIG_SCHEMA = cover.COVER_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(SomfyCoverComponent),
    cv.Required(CONF_ADDRESS): cv.uint32_t
}).extend(somfy.SOMFY_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_address(config[CONF_ADDRESS]))
    yield cg.register_component(var, config)
    yield cover.register_cover(var, config)
    yield somfy.register_somfy_device(var, config)
