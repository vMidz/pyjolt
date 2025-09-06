from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class DampingTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        sphere = pyjolt.SphereShape(2.0)

        # Bodies with increasing linear damping
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                sphere, 
                Vec3(-50.0 + i * 10.0, 2.0, -80.0), 
                Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, 
                Layers.MOVING
            )
            body = body_interface.create_body(settings)
            
            motion_properties = body.get_motion_properties()
            motion_properties.set_angular_damping(0.0)
            motion_properties.set_linear_damping(0.1 * i)
            
            body.linear_velocity = Vec3(0, 0, 10)
            body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)

        # Bodies with increasing angular damping
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                sphere, 
                Vec3(-50.0 + i * 10.0, 2.0, -90.0), 
                Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, 
                Layers.MOVING
            )
            body = body_interface.create_body(settings)

            motion_properties = body.get_motion_properties()
            motion_properties.set_linear_damping(0.0)
            motion_properties.set_angular_damping(0.1 * i)
            
            body.angular_velocity = Vec3(0, 10, 0)
            body_interface.add_body(body.get_id(), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()