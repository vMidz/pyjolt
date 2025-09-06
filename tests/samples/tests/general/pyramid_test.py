from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class PyramidTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        c_box_size = 2.0
        c_box_separation = 0.5
        c_half_box_size = 0.5 * c_box_size
        c_pyramid_height = 15

        box_shape = pyjolt.BoxShape(Vec3.replicate(c_half_box_size))

        # Pyramid
        for i in range(c_pyramid_height):
            start_jk = i // 2
            end_jk = c_pyramid_height - (i + 1) // 2
            
            for j in range(start_jk, end_jk):
                for k in range(start_jk, end_jk):
                    # (i & 1) is a check for odd rows
                    offset = c_half_box_size if (i & 1) else 0.0

                    position = Vec3(
                        -c_pyramid_height + c_box_size * j + offset,
                        1.0 + (c_box_size + c_box_separation) * i,
                        -c_pyramid_height + c_box_size * k + offset
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