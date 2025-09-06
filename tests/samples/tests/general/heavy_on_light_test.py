from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class HeavyOnLightTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        # The first box shape is created once with a constant density
        box1 = pyjolt.BoxShape(Vec3.replicate(5.0))
        box1.set_density(1000.0)

        for i in range(1, 11):
            # Create the first body (the one on the bottom)
            pos1 = Vec3(-75.0 + i * 15.0, 10.0, 0.0)
            settings1 = pyjolt.BodyCreationSettings(
                box1, pos1, Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING
            )
            body_interface.create_and_add_body(settings1, pyjolt.EActivation.ACTIVATE)

            # Create a new shape for the second box in each iteration
            # because its density changes
            box2 = pyjolt.BoxShape(Vec3.replicate(5.0))
            box2.set_density(5000.0 * i)

            # Create the second body (the heavy one on top)
            pos2 = Vec3(-75.0 + i * 15.0, 30.0, 0.0)
            settings2 = pyjolt.BodyCreationSettings(
                box2, pos2, Quat.identity(), 
                pyjolt.EMotionType.DYNAMIC, Layers.MOVING
            )
            body_interface.create_and_add_body(settings2, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()
