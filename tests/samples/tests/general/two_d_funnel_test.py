from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class TwoDFunnelTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        wall = pyjolt.BoxShape(Vec3(0.1, 10.0, 1.0))

        # 2D funnel
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            wall, Vec3(-12, 8, -5), Quat.rotation(Vec3.axis_z(), 0.2 * math.JPH_PI),
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)
            
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            wall, Vec3(12, 8, -5), Quat.rotation(Vec3.axis_z(), -0.2 * math.JPH_PI),
            pyjolt.EMotionType.STATIC, Layers.NON_MOVING), pyjolt.EActivation.DONT_ACTIVATE)

        # Shapes falling in 2D funnel
        shapes = [
            pyjolt.SphereShape(0.5),
            pyjolt.BoxShape(Vec3.replicate(0.5)),
            pyjolt.CapsuleShape(0.2, 0.3)
        ]
        
        bcs = pyjolt.BodyCreationSettings(
            shapes[0], Vec3.zero(), Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
            
        bcs.allowed_dofs = pyjolt.EAllowedDOFs.PLANE2D
        
        for x in range(20):
            for y in range(10):
                bcs.set_shape(shapes[(x * y) % len(shapes)])
                bcs.position = Vec3(-10.0 + x, 10.0 + y, -5.0)
                body_interface.create_and_add_body(bcs, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()