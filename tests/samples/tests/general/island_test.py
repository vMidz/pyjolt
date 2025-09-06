from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class IslandTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        box_shape = pyjolt.BoxShape(Vec3(1.0, 1.0, 1.0))

        # Walls
        for i in range(10):
            # C++ integer division is `//` in Python
            start_j = i // 2
            end_j = 10 - (i + 1) // 2
            for j in range(start_j, end_j):
                for k in range(8):
                    # (i & 1) checks if i is odd, same as (i % 2 != 0)
                    offset = 1.0 if (i & 1) else 0.0
                    position = Vec3(
                        -10.0 + j * 2.0 + offset,
                        1.0 + i * 2.0,
                        8.0 * (k - 4)
                    )
                    
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