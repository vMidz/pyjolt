from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class Listener(pyjolt.BodyActivationListener):
    def on_body_activated(self, body_id: pyjolt.BodyID, body_user_data: int) -> None:
        pyjolt.trace(f"Body {body_id.get_index()} activated")

    def on_body_deactivated(self, body_id: pyjolt.BodyID, body_user_data: int) -> None:
        pyjolt.trace(f"Body {body_id.get_index()} deactivated")

class SimpleTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()
        self.listener = Listener()
        self.physics_system.set_body_activation_listener(self.listener)

        self.create_floor()

        box_shape = pyjolt.BoxShape(Vec3(0.5, 1.0, 2.0))
        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            box_shape, Vec3(0, 10, 0), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            box_shape, Vec3(5, 10, 0),
            Quat.rotation(Vec3().axis_x(), 0.25 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

        body_interface.create_and_add_body(pyjolt.BodyCreationSettings(
            pyjolt.SphereShape(2.0), Vec3(10, 10, 0), Quat.rotation(Vec3.axis_x(),
            0.25 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING), pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        self.physics_system.set_body_activation_listener(None)
        super().cleanup()