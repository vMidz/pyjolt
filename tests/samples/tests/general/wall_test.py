from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class WallTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        box_shape = pyjolt.BoxShape(Vec3(1.0, 1.0, 1.0))

        # Wall
        for i in range(10):
            start_j = i // 2
            end_j = 50 - (i + 1) // 2
            for j in range(start_j, end_j):
                # (i & 1) checks if i is odd, same as (i % 2 != 0)
                offset = 1.0 if (i & 1) else 0.0
                position = Vec3(-50.0 + j * 2.0 + offset, 1.0 + i * 3.0, 0.0)
                
                settings = pyjolt.BodyCreationSettings(
                    box_shape, 
                    position, 
                    Quat.identity(), 
                    pyjolt.EMotionType.DYNAMIC, 
                    Layers.MOVING
                )
                body_interface.create_and_add_body(settings, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()