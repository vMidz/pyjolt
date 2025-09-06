
from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3
from tests.general.load_save_scene_test import LoadSaveSceneTest 
from layers import ObjectVsBroadPhaseLayerFilterImpl, ObjectLayerPairFilterImpl, BPLayerInterfaceImpl
import io

class LoadSaveBinaryTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)

        # Create scene
        scene = LoadSaveSceneTest.create_scene()

        # Create a new scene by instantiating the scene in a physics system and then converting it back to a scene
        temp_system = pyjolt.PhysicsSystem()
        layer_interface = BPLayerInterfaceImpl()
        object_vs_broadphase_layer_filter = ObjectVsBroadPhaseLayerFilterImpl()
        object_vs_object_layer_filter = ObjectLayerPairFilterImpl()
        
        temp_system.init(
            self.physics_system.get_max_bodies(), 0, 1024, 1024,
            layer_interface, 
            object_vs_broadphase_layer_filter, 
            object_vs_object_layer_filter
        )
        
        scene.create_bodies(temp_system)
        
        scene_copy = pyjolt.PhysicsScene()
        scene_copy.from_physics_system(temp_system)

        # Replace the original scene
        scene = scene_copy

        # Use an in-memory binary stream
        buffer = io.BytesIO()

        # Write scene
        scene.save_binary_state(buffer, True, True)

        # Clear scene
        scene = None

        # Read scene back in
        # Reset stream to the beginning before reading
        buffer.seek(0)
        
        result = pyjolt.PhysicsScene.restore_from_binary_state(buffer)
        if result.has_error():
            # Raising an exception is a Pythonic way to handle a fatal error
            raise RuntimeError(result.get_error())
        scene = result.get()

        # Instantiate scene
        scene.create_bodies(self.physics_system)

    def cleanup(self):
        constraints = self.physics_system.get_constraints()
        for constraint in constraints:
            self.physics_system.remove_constraint(constraint.get())
        super().cleanup()