from tests.test_common import Test, Layers, Context
import math
import pyjolt
from pyjolt.math import Vec3, Quat

class ChangeMotionTypeTest(Test):
    # Static constant for the motion type cycle
    CYCLE = [
        pyjolt.EMotionType.DYNAMIC,
        pyjolt.EMotionType.KINEMATIC,
        pyjolt.EMotionType.STATIC,
        pyjolt.EMotionType.KINEMATIC,
        pyjolt.EMotionType.DYNAMIC,
        pyjolt.EMotionType.STATIC
    ]

    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Initialize member variables
        self.time = 0.0
        self.body = None

        # Floor
        self.create_floor()

        # Create body as static, but allow to become dynamic
        settings = pyjolt.BodyCreationSettings()
        settings.set_shape(pyjolt.BoxShape(Vec3(0.5, 1.0, 2.0)))
        settings.position = Vec3(0, 10, 0)
        settings.motion_type = pyjolt.EMotionType.DYNAMIC
        settings.object_layer = Layers.MOVING  # Put in moving layer, this will result in some overhead when the body is static
        settings.allow_dynamic_or_kinematic = True
        
        self.body = body_interface.create_body(settings)
        body_interface.add_body(self.body.get_id(), pyjolt.EActivation.ACTIVATE)

    def update_motion_type(self):
        # Calculate desired motion type
        motion_type = self.CYCLE[int(self.time) % len(self.CYCLE)]

        # Update motion type and reactivate the body
        if motion_type != self.body.motion_type:
            body_interface = self.physics_system.get_body_interface()
            body_interface.set_motion_type(self.body.get_id(), motion_type, pyjolt.EActivation.ACTIVATE)

    def pre_physics_update(self, delta_time) -> None:
        # Increment time
        self.time += delta_time

        self.update_motion_type()

        # Provide kinematic body a target
        if self.body.is_kinematic():
            target_position = Vec3(math.sin(self.time), 10, 0)
            target_rotation = Quat.rotation(Vec3.axis_x(), math.cos(self.time))
            self.body.move_kinematic(target_position, target_rotation, delta_time)

    def save_state(self, stream):
        stream.write(self.time)

    def restore_state(self, stream):
        stream.read(self.time)
        self.update_motion_type()

    def cleanup(self):
        super().cleanup()