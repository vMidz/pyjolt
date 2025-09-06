from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class GravityFactorTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        box_shape = pyjolt.BoxShape(Vec3(2.0, 2.0, 2.0))

        # Bodies with increasing gravity fraction
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                box_shape, 
                Vec3(-50.0 + i * 10.0, 25.0, 0), 
                Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, 
                Layers.MOVING
            )
            body = body_interface.create_body(settings)
            
            motion_properties = body.get_motion_properties()
            motion_properties.set_gravity_factor(0.1 * i)
            
            body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()