from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class StackTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()
        self.physics_system.set_body_activation_listener(None)

        self.create_floor()

        box_shape = pyjolt.BoxShape(Vec3(0.5,1.0,2.0)) 

        for i in range(10):
            rotation=Quat()
            if (i & 1) != 0:
                rotation=Quat.rotation(Vec3.axis_y(), 0.5*pyjolt.math.JPH_PI)
            else:
                rotation=Quat.identity()

            body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
                    box_shape, Vec3(10.0, 1.0+i*2.1, 0), rotation, pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()
