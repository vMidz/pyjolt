from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class KinematicTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Initialize member variables
        self.kinematic = [None, None]

        # Floor
        self.create_floor()

        # Wall
        box_shape = pyjolt.BoxShape(Vec3(1.0, 1.0, 1.0))
        for i in range(3):
            start_j = i // 2
            end_j = 10 - (i + 1) // 2
            for j in range(start_j, end_j):
                offset = 1.0 if (i & 1) else 0.0
                position = Vec3(-10.0 + j * 2.0 + offset, 1.0 + i * 2.0, 0)
                
                settings = pyjolt.BodyCreationSettings(
                    box_shape, 
                    position, 
                    Quat.identity(), 
                    pyjolt.EMotionType.DYNAMIC, 
                    Layers.MOVING
                )
                body_interface.create_and_add_body(settings, pyjolt.EActivation.DONT_ACTIVATE)

        # Kinematic object
        for i in range(2):
            z_pos = 5.0 if i == 0 else -5.0
            settings = pyjolt.BodyCreationSettings(
                pyjolt.SphereShape(1.0), 
                Vec3(-10.0, 2.0, z_pos), 
                Quat.identity(), 
                pyjolt.EMotionType.KINEMATIC, 
                Layers.MOVING
            )
            self.kinematic[i] = body_interface.create_body(settings)
            body_interface.add_body(self.kinematic[i].get_id(), pyjolt.EActivation.ACTIVATE)

    def pre_physics_update(self, delta_time) -> None:
        for i in range(2):
            com = self.kinematic[i].get_center_of_mass_position()
            if com.z >= 5.0:
                self.kinematic[i].linear_velocity = Vec3(2.0, 0, -10.0)
            elif com.z <= -5.0:
                self.kinematic[i].linear_velocity = Vec3(2.0, 0, 10.0)

    def cleanup(self):
        super().cleanup()