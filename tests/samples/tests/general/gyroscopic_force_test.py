from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class GyroscopicForceTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        compound = pyjolt.StaticCompoundShapeSettings()
        compound.add_shape(Vec3.zero(), Quat.identity(), pyjolt.BoxShape(Vec3(0.5, 5.0, 0.5)))
        compound.add_shape(Vec3(1.5, 0, 0), Quat.identity(), pyjolt.BoxShape(Vec3(1.0, 0.5, 0.5)))
        
        # Note: SetEmbedded() is a C++ memory optimization and is not used in the Python bindings.

        # One body without gyroscopic force
        bcs = pyjolt.BodyCreationSettings(
            compound, 
            Vec3(0, 10, 0), 
            Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, 
            Layers.MOVING
        )
        bcs.linear_damping = 0.0
        bcs.angular_damping = 0.0
        bcs.angular_velocity = Vec3(10, 1, 0)
        bcs.gravity_factor = 0.0
        body_interface.create_and_add_body(bcs, pyjolt.EActivation.ACTIVATE)

        # One body with gyroscopic force
        bcs.position += Vec3(10, 0, 0)
        bcs.apply_gyroscopic_force = True
        body_interface.create_and_add_body(bcs, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        super().cleanup()
