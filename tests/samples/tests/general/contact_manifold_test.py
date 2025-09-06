from pyjolt import math
from pyjolt.math import Vec3, Quat
from tests.test_common import Test, Layers, Context
import pyjolt

class ContactManifoldTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        big_box = pyjolt.BoxShape(Vec3(4.0, 4.0, 4.0), 0.0)
        capsule = pyjolt.CapsuleShape(5.0, 2.0)
        long_box = pyjolt.BoxShape(Vec3(2.0, 7.0, 2.0))

        for i in range(3):
            for j in range(2):
                # Create a box
                static_settings = pyjolt.BodyCreationSettings(
                    big_box, 
                    Vec3(-20.0 + i * 10.0, 4.0, -20.0 + j * 40.0), 
                    Quat.identity(), 
                    pyjolt.EMotionType.STATIC, 
                    Layers.NON_MOVING
                )
                body_interface.create_and_add_body(static_settings, pyjolt.EActivation.DONT_ACTIVATE)

                # Place a dynamic body on it
                dynamic_shape = capsule if j == 0 else long_box
                position = Vec3(-20.0 + i * 10.0, 12.0, -5.0 + i * 5.0 - 20.0 + j * 40.0)
                rotation = Quat.rotation(Vec3.axis_y(), 0.1 * math.JPH_PI) * Quat.rotation(Vec3.axis_x(), 0.5 * math.JPH_PI)
                
                dynamic_settings = pyjolt.BodyCreationSettings(
                    dynamic_shape,
                    position,
                    rotation,
                    pyjolt.EMotionType.DYNAMIC,
                    Layers.MOVING
                )
                body_interface.create_and_add_body(dynamic_settings, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()