from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3

class ActivateDuringUpdateTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        settings = pyjolt.BodyCreationSettings()
        settings.set_shape(pyjolt.BoxShape(Vec3.replicate(0.5)))
        settings.motion_type = pyjolt.EMotionType.DYNAMIC
        settings.object_layer = Layers.MOVING

        c_num_bodies = 3
        c_penetration_slop = self.physics_system.get_physics_settings().penetration_slop

        for i in range(c_num_bodies):
            settings.position = Vec3(i * (1.0 - c_penetration_slop), 2.0, 0.0)
            body_id = body_interface.create_body(settings).get_id()
            body_interface.add_body(body_id, pyjolt.EActivation.DONT_ACTIVATE)
            if i == 0:
                body_interface.set_linear_velocity(body_id, Vec3(500, 0, 0))

        for i in range(c_num_bodies):
            settings.position = Vec3(i * (1.0 - c_penetration_slop), 2.0, 2.0)
            body_id = body_interface.create_body(settings).get_id()
            body_interface.add_body(body_id, pyjolt.EActivation.DONT_ACTIVATE)
            if i == c_num_bodies - 1:
                body_interface.set_linear_velocity(body_id, Vec3(-500, 0, 0))

    def cleanup(self):
        super().cleanup()