import pyjolt
from enum import IntEnum

# Layer that objects can be in, determines which other objects it can collide with
class Layers(IntEnum):
    UNUSED1 = 0           # 4 unused values so that broadphase layers values don't match with object layer values (for testing purposes)
    UNUSED2 = 1
    UNUSED3 = 2
    UNUSED4 = 3
    NON_MOVING = 4
    MOVING = 5
    DEBRIS = 6            # Example: Debris collides only with NON_MOVING
    SENSOR = 7            #  Sensors only collide with MOVING objects
    NUM_LAYERS = 8

class BroadPhaseLayers(IntEnum):
    NON_MOVING = 0
    MOVING = 1
    DEBRIS = 2
    SENSOR = 3
    UNUSED = 4
    NUM_LAYERS = 5

class ObjectLayerPairFilterImpl(pyjolt.ObjectLayerPairFilter):
    def should_collide(self, object_1: int, object_2: int) -> bool:
        if object_1 in (Layers.UNUSED1, Layers.UNUSED2, Layers.UNUSED3, Layers.UNUSED4):
            return False
        elif object_1 == Layers.NON_MOVING:
            return object_2 in (Layers.MOVING, Layers.DEBRIS)
        elif object_1 == Layers.MOVING:
            return object_2 in (Layers.NON_MOVING, Layers.MOVING, Layers.SENSOR)
        elif object_1 == Layers.DEBRIS:
            return object_2 == Layers.NON_MOVING
        elif object_1 == Layers.SENSOR:
            return object_2 == Layers.MOVING
        else:
            assert False, "Invalid layer"
            return False
 
class BPLayerInterfaceImpl(pyjolt.BroadPhaseLayerInterface):
    def __init__(self):
        super().__init__()
        self.object_to_broad_phase = [pyjolt.BroadPhaseLayer] * Layers.NUM_LAYERS
        self.object_to_broad_phase[Layers.UNUSED1] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.UNUSED)
        self.object_to_broad_phase[Layers.UNUSED2] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.UNUSED)
        self.object_to_broad_phase[Layers.UNUSED3] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.UNUSED)
        self.object_to_broad_phase[Layers.UNUSED4] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.UNUSED)
        self.object_to_broad_phase[Layers.NON_MOVING] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.NON_MOVING)
        self.object_to_broad_phase[Layers.MOVING] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.MOVING)
        self.object_to_broad_phase[Layers.DEBRIS] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.DEBRIS)
        self.object_to_broad_phase[Layers.SENSOR] = pyjolt.BroadPhaseLayer(BroadPhaseLayers.SENSOR)

    def get_num_broad_phase_layers(self) -> int:
        return BroadPhaseLayers.NUM_LAYERS

    def get_broad_phase_layer(self, layer: int) -> pyjolt.BroadPhaseLayer:
        assert 0 <= layer < Layers.NUM_LAYERS, "Invalid object layer"
        return self.object_to_broad_phase[layer]

    def get_broad_phase_layer_name(self, layer: pyjolt.BroadPhaseLayer) -> str:
        mapping = {
            BroadPhaseLayers.NON_MOVING: "NON_MOVING",
            BroadPhaseLayers.MOVING: "MOVING",
            BroadPhaseLayers.DEBRIS: "DEBRIS",
            BroadPhaseLayers.SENSOR: "SENSOR",
            BroadPhaseLayers.UNUSED: "UNUSED"
        }
        return mapping.get(layer.get_value(), "INVALID")

class ObjectVsBroadPhaseLayerFilterImpl(pyjolt.ObjectVsBroadPhaseLayerFilter):
    def should_collide(self, layer_1: int, layer_2: pyjolt.BroadPhaseLayer) -> bool:
        layer_type = layer_2.get_value()

        if layer_1 == Layers.NON_MOVING:
            return layer_type in (BroadPhaseLayers.MOVING, BroadPhaseLayers.DEBRIS)
        elif layer_1 == Layers.MOVING:
            return layer_type in (
                BroadPhaseLayers.NON_MOVING,
                BroadPhaseLayers.MOVING,
                BroadPhaseLayers.SENSOR
            )
        elif layer_1 == Layers.DEBRIS:
            return layer_type == BroadPhaseLayers.NON_MOVING
        elif layer_1 == Layers.SENSOR:
            return layer_type == BroadPhaseLayers.MOVING
        elif layer_1 in (Layers.UNUSED1, Layers.UNUSED2, Layers.UNUSED3):
            return False
        else:
            assert False, "Invalid object layer"
            return False
