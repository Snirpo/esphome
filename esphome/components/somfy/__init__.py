from esphome.core import coroutine
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.remote_base import CONF_TRANSMITTER_ID, RemoteTransmitterBase
from esphome.components.cc1101 import CONF_CC1101_ID

somfy_ns = cg.esphome_ns.namespace('somfy')
SomfyComponent = somfy_ns.class_('SomfyComponent', cg.Component)

DEPENDENCIES = ['remote_transmitter']

CONF_SOMFY_ID = 'somfy_id'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SOMFY_ID): cv.declare_id(SomfyComponent),
    cv.GenerateID(CONF_TRANSMITTER_ID): cv.use_id(RemoteTransmitterBase)
}).extend(cv.COMPONENT_SCHEMA)

SOMFY_DEVICE_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_SOMFY_ID): cv.use_id(SomfyComponent)
})


@coroutine
def register_somfy_device(var, config):
    parent = yield cg.get_variable(config[CONF_SOMFY_ID])
    cg.add(var.set_somfy_parent(parent))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_SOMFY_ID])
    yield cg.register_component(var, config)

    # TODO: move?
    transmitter = yield cg.get_variable(config[CONF_TRANSMITTER_ID])
    cg.add(var.set_transmitter(transmitter))

    # TODO: not hardcoded
    cc1101 = yield cg.get_variable(config[CONF_CC1101_ID])
    cg.add(var.set_configurer(cc1101))
