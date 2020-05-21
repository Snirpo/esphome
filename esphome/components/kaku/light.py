import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import remote_base, light
from esphome.const import CONF_OUTPUT_ID
from esphome.components.remote_base import CONF_RECEIVER_ID, CONF_TRANSMITTER_ID, RemoteReceiverBase, \
    RemoteTransmitterBase

kaku_ns = cg.esphome_ns.namespace('kaku')
KakuComponent = kaku_ns.class_('KakuComponent', light.LightOutput, remote_base.RemoteReceiverListener, cg.Component)

DEPENDENCIES = ['remote_receiver']

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(KakuComponent),
    cv.GenerateID(CONF_RECEIVER_ID): cv.use_id(RemoteReceiverBase),
    # cv.GenerateID(CONF_TRANSMITTER_ID): cv.use_id(RemoteTransmitterBase)
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    # transmitter = yield cg.get_variable(config[CONF_TRANSMITTER_ID])

    yield remote_base.register_listener(var, config)
    yield light.register_light(var, config)
