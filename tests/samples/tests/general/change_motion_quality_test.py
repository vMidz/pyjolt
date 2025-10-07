from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat, JPH_PI

class ChangeMotionQualityTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Initialize member variables
        self.time = 0.0
        self.body = None

        # Floor
        self.create_floor()

        # Single shape that has 4 walls to surround fast moving sphere
        box_shape_settings = pyjolt.BoxShapeSettings(Vec3(5.0, 1.0, 0.1))
        enclosing_shape_settings = pyjolt.StaticCompoundShapeSettings()
        enclosing_shape_settings.add_shape(Vec3(0, 0, 5), Quat.identity(), box_shape_settings)
        enclosing_shape_settings.add_shape(Vec3(0, 0, -5), Quat.identity(), box_shape_settings)
        enclosing_shape_settings.add_shape(Vec3(5, 0, 0), Quat.rotation(Vec3.axis_y(), 0.5 * JPH_PI), box_shape_settings)
        enclosing_shape_settings.add_shape(Vec3(-5, 0, 0), Quat.rotation(Vec3.axis_y(), 0.5 * JPH_PI), box_shape_settings)
        
        enclosing_settings = pyjolt.BodyCreationSettings()
        enclosing_settings.set_shape_settings(enclosing_shape_settings)
        enclosing_settings.motion_type = pyjolt.EMotionType.KINEMATIC
        enclosing_settings.object_layer = Layers.MOVING
        enclosing_settings.position = Vec3(0, 1, 0)
        body_interface.create_and_add_body(enclosing_settings, pyjolt.EActivation.ACTIVATE)

        # Create high speed sphere inside
        settings = pyjolt.BodyCreationSettings()
        settings.set_shape(pyjolt.SphereShape(1.0))
        settings.position = Vec3(0, 0.5, 0)
        settings.motion_type = pyjolt.EMotionType.DYNAMIC
        settings.motion_quality = pyjolt.EMotionQuality.LINEAR_CAST
        settings.linear_velocity = Vec3(-240, 0, -120)
        settings.friction = 0.0
        settings.restitution = 1.0
        settings.object_layer = Layers.MOVING
        
        self.body = body_interface.create_body(settings)
        body_interface.add_body(self.body.get_id(), pyjolt.EActivation.ACTIVATE)

    def update_motion_quality(self):
        # Calculate desired motion quality
        if int(self.time) % 2 == 0:
            motion_quality = pyjolt.EMotionQuality.LINEAR_CAST
        else:
            motion_quality = pyjolt.EMotionQuality.DISCRETE
        
        body_interface = self.physics_system.get_body_interface()
        body_interface.set_motion_quality(self.body.get_id(), motion_quality)

    def pre_physics_update(self, delta_time) -> None:
        # Increment time
        self.time += delta_time

        self.update_motion_quality()
        
    def save_state(self, stream):
        stream.write(self.time)

    def restore_state(self, stream):
        stream.read(self.time)
        self.update_motion_quality()

    def cleanup(self):
        super().cleanup()