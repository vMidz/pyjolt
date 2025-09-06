from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt.math import Vec3, Quat

class ChangeShapeTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Initialize member variables
        self.time = 0.0
        self.shape_idx = 0
        self.shapes = []
        self.body_id = None
        self.activate_after_switch = True # This member is used but not initialized in C++, assuming it's a test param

        # Floor
        self.create_floor()

        # Shapes
        self.shapes.append(pyjolt.BoxShape(Vec3(0.5, 1.5, 0.5)))
        self.shapes.append(pyjolt.SphereShape(0.5))
        self.shapes.append(pyjolt.CapsuleShape(1.0, 0.5))
        
        tapered_capsule_settings = pyjolt.TaperedCapsuleShapeSettings(1.0, 0.5, 0.3)
        self.shapes.append(tapered_capsule_settings.create().get())

        # Compound with center of mass shifted (this requires a correction of the position in the body)
        compound_settings = pyjolt.StaticCompoundShapeSettings()
        compound_settings.add_shape(Vec3(0, 1.5, 0), Quat.identity(), pyjolt.CapsuleShape(1.5, 0.5))
        compound_settings.add_shape(Vec3(0, 3.0, 0), Quat.identity(), pyjolt.SphereShape(1.0))
        compound_shape = compound_settings.create().get()
        self.shapes.append(compound_shape)

        # Create dynamic body that changes shape
        settings = pyjolt.BodyCreationSettings()
        settings.set_shape(self.shapes[self.shape_idx])
        settings.position = Vec3(0, 10, 0)
        settings.motion_type = pyjolt.EMotionType.DYNAMIC
        settings.object_layer = Layers.MOVING
        
        body = body_interface.create_body(settings)
        self.body_id = body.get_id()
        body_interface.add_body(self.body_id, pyjolt.EActivation.ACTIVATE)

    def pre_physics_update(self,  delta_time) -> None:
        c_switch_time = 3.0
        body_interface = self.physics_system.get_body_interface()

        # Increment time
        self.time += delta_time

        # Get new shape
        shape_idx = int(self.time / c_switch_time) % len(self.shapes)

        # Change shape
        if self.shape_idx != shape_idx:
            self.shape_idx = shape_idx
            
            activation = pyjolt.EActivation.ACTIVATE if self.activate_after_switch else pyjolt.EActivation.DONT_ACTIVATE
            body_interface.set_shape(self.body_id, self.shapes[self.shape_idx], True, activation)

    def save_state(self, out_stream) -> None:
        out_stream.write_float(self.time)
        out_stream.write_int(self.shape_idx)

    def restore_state(self, in_stream) -> None:
        self.time = in_stream.read_float()
        self.shape_idx = in_stream.read_int()

        # Reset the shape to what was stored
        body_interface = self.physics_system.get_body_interface()
        body_interface.set_shape(self.body_id, self.shapes[self.shape_idx], True, pyjolt.EActivation.DONT_ACTIVATE)

    def cleanup(self):
        super().cleanup()


# TODO: create_settings_menu