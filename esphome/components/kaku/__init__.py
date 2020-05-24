from esphome.core import coroutine
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import remote_base
from esphome.components.remote_base import CONF_RECEIVER_ID, CONF_TRANSMITTER_ID, RemoteReceiverBase, \
    RemoteTransmitterBase

kaku_ns = cg.esphome_ns.namespace('kaku')
KakuComponent = kaku_ns.class_('KakuComponent', remote_base.RemoteReceiverListener, cg.Component)

DEPENDENCIES = ['remote_receiver', 'remote_transmitter']

CONF_KAKU_ID = 'kaku_id'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_KAKU_ID): cv.declare_id(KakuComponent),
    cv.GenerateID(CONF_RECEIVER_ID): cv.use_id(RemoteReceiverBase),
    cv.GenerateID(CONF_TRANSMITTER_ID): cv.use_id(RemoteTransmitterBase)
}).extend(cv.COMPONENT_SCHEMA)

KAKU_DEVICE_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_KAKU_ID): cv.use_id(KakuComponent)
})


@coroutine
def register_kaku_device(var, config):
    parent = yield cg.get_variable(config[CONF_KAKU_ID])
    cg.add(var.set_kaku_parent(parent))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_KAKU_ID])
    yield cg.register_component(var, config)

    # TODO: move?
    transmitter = yield cg.get_variable(config[CONF_TRANSMITTER_ID])
    cg.add(var.set_transmitter(transmitter))

    yield remote_base.register_listener(var, config)
