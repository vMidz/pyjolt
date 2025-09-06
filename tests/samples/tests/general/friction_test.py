from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class FrictionTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Common rotation for all bodies
        ramp_rotation = Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI)

        # Floor
        floor_settings = pyjolt.BodyCreationSettings(
            pyjolt.BoxShape(Vec3(100.0, 1.0, 100.0), 0.0),
            Vec3.zero(),
            ramp_rotation,
            pyjolt.EMotionType.STATIC,
            Layers.NON_MOVING
        )
        floor = body_interface.create_body(floor_settings)
        floor.friction = 1.0
        body_interface.add_body(floor.get_id(), pyjolt.EActivation.DONT_ACTIVATE)

        box_shape = pyjolt.BoxShape(Vec3(2.0, 2.0, 2.0))
        sphere_shape = pyjolt.SphereShape(2.0)

        # Bodies with increasing friction (boxes)
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                box_shape,
                Vec3(-50.0 + i * 10.0, 55.0, -50.0),
                ramp_rotation,
                pyjolt.EMotionType.DYNAMIC,
                Layers.MOVING
            )
            body = body_interface.create_body(settings)
            body.friction = 0.1 * i
            body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)

        # Bodies with increasing friction (spheres)
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                sphere_shape,
                Vec3(-50.0 + i * 10.0, 47.0, -40.0),
                ramp_rotation,
                pyjolt.EMotionType.DYNAMIC,
                Layers.MOVING
            )
            body = body_interface.create_body(settings)
            body.friction = 0.1 * i
            body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)
            
    def cleanup(self):
        super().cleanup()