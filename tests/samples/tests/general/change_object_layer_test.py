from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat
import random

class ChangeObjectLayerTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Initialize member variables
        self.time = 0.0
        self.is_debris = True
        self.moving = None
        self.debris = []

        # Floor
        self.create_floor()

        # A dynamic box in the MOVING layer
        moving_settings = pyjolt.BodyCreationSettings(
            pyjolt.BoxShape(Vec3(5, 0.1, 5)), 
            Vec3(0, 1.5, 0), 
            Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, 
            Layers.MOVING
        )
        self.moving = body_interface.create_and_add_body(moving_settings, pyjolt.EActivation.ACTIVATE)

        # Lots of dynamic objects in the DEBRIS layer
        debris_shape = pyjolt.BoxShape(Vec3.replicate(0.1))
        for _ in range(50):
            position = Vec3(random.uniform(-10, 10), 2.0, random.uniform(-10, 10))
            # Assuming Quat.random() is provided by the binding for a uniform random rotation
            rotation = Quat.random()
            debris_settings = pyjolt.BodyCreationSettings(
                debris_shape, 
                position, 
                rotation, 
                pyjolt.EMotionType.DYNAMIC, 
                Layers.DEBRIS
            )
            body_id = body_interface.create_and_add_body(debris_settings, pyjolt.EActivation.ACTIVATE)
            self.debris.append(body_id)

    def pre_physics_update(self, delta_time) -> None:
        c_switch_time = 2.0
        body_interface = self.physics_system.get_body_interface()

        # Increment time
        self.time += delta_time

        if self.time >= c_switch_time:
            self.is_debris = not self.is_debris

            # Reposition moving object
            body_interface.set_position(self.moving, Vec3(0, 1.5, 0), pyjolt.EActivation.ACTIVATE)

            for body_id in self.debris:
                # Reposition debris
                position = Vec3(random.uniform(-7.5, 7.5), 2.0, random.uniform(-7.5, 7.5))
                rotation = Quat.random()
                body_interface.set_position_and_rotation(body_id, position, rotation, pyjolt.EActivation.ACTIVATE)

                # And update layer
                new_layer = Layers.DEBRIS if self.is_debris else Layers.MOVING
                body_interface.set_object_layer(body_id, new_layer)

            self.time = 0.0

    def save_state(self, out_stream) -> None:
        out_stream.write_float(self.time)
        out_stream.write_bool(self.is_debris)

    def restore_state(self, in_stream) -> None:
        self.time = in_stream.read_float()
        self.is_debris = in_stream.read_bool()

        body_interface = self.physics_system.get_body_interface()
        
        # Restore layer
        new_layer = Layers.DEBRIS if self.is_debris else Layers.MOVING
        for body_id in self.debris:
            body_interface.set_object_layer(body_id, new_layer)

    def cleanup(self):
        super().cleanup()