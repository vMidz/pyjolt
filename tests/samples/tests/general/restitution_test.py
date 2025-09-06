from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class RestitutionTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        sphere = pyjolt.SphereShape(2.0)
        box = pyjolt.BoxShape(Vec3(2.0, 2.0, 2.0))

        # Bodies with increasing restitution (spheres)
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                sphere, 
                Vec3(-50.0 + i * 10.0, 20.0, -20.0), 
                Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, 
                Layers.MOVING
            )
            settings.restitution = 0.1 * i
            settings.linear_damping = 0.0
            body_interface.create_and_add_body(settings, pyjolt.EActivation.ACTIVATE)

        # Bodies with increasing restitution (boxes)
        for i in range(11):
            settings = pyjolt.BodyCreationSettings(
                box, 
                Vec3(-50.0 + i * 10.0, 20.0, 20.0), 
                Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, 
                Layers.MOVING
            )
            settings.restitution = 0.1 * i
            settings.linear_damping = 0.0
            body_interface.create_and_add_body(settings, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()