from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat

class ContactListenerImpl(pyjolt.ContactListener):
    def __init__(self, test_instance: 'ConveyorBeltTest'):
        super().__init__()
        self.test_instance = test_instance

    def on_contact_added(self, body1: pyjolt.Body, body2: pyjolt.Body, manifold: pyjolt.ContactManifold, settings: pyjolt.ContactSettings) -> None:
        # Linear belts
        body1_is_linear_belt = body1.get_id() in self.test_instance.linear_belts
        body2_is_linear_belt = body2.get_id() in self.test_instance.linear_belts
        if body1_is_linear_belt or body2_is_linear_belt:
            c_local_space_velocity = Vec3(0, 0, -10.0)
            body1_linear_surface_velocity = body1.get_rotation() * c_local_space_velocity if body1_is_linear_belt else Vec3.zero()
            body2_linear_surface_velocity = body2.get_rotation() * c_local_space_velocity if body2_is_linear_belt else Vec3.zero()
            settings.relative_linear_surface_velocity = body2_linear_surface_velocity - body1_linear_surface_velocity

        # Angular belt
        body1_is_angular = body1.get_id() == self.test_instance.angular_belt
        body2_is_angular = body2.get_id() == self.test_instance.angular_belt
        if body1_is_angular or body2_is_angular:
            c_local_space_angular_velocity = Vec3(0, math.degrees_to_radians(10.0), 0)
            body1_angular_surface_velocity = body1.get_rotation() * c_local_space_angular_velocity if body1_is_angular else Vec3.zero()
            body2_angular_surface_velocity = body2.get_rotation() * c_local_space_angular_velocity if body2_is_angular else Vec3.zero()
            
            # Note that the angular velocity is the angular velocity around body 1's center of mass, so we need to add the linear velocity of body 2's center of mass
            body2_linear_surface_velocity = body2_angular_surface_velocity.cross(body1.get_center_of_mass_position() - body2.get_center_of_mass_position()) if body2_is_angular else Vec3.zero()
            
            settings.relative_linear_surface_velocity = body2_linear_surface_velocity
            settings.relative_angular_surface_velocity = body2_angular_surface_velocity - body1_angular_surface_velocity

    def on_contact_persisted(self, body1: pyjolt.Body, body2: pyjolt.Body, manifold: pyjolt.ContactManifold, settings: pyjolt.ContactSettings) -> None:
        # Same behavior as contact added
        self.on_contact_added(body1, body2, manifold, settings)


class ConveyorBeltTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        self.listener = ContactListenerImpl(self)

        ctx.physics_system.set_contact_listener(self.listener)

        # Initialize member variables
        self.linear_belts = []
        self.angular_belt = None

        # Floor
        self.create_floor()

        # Create conveyor belts
        c_belt_width = 10.0
        c_belt_length = 50.0
        belt_shape = pyjolt.BoxShape(Vec3(c_belt_width, 0.1, c_belt_length))
        belt_settings = pyjolt.BodyCreationSettings(belt_shape, Vec3.zero(), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        
        for i in range(4):
            belt_settings.friction = 0.25 * (i + 1)
            belt_settings.rotation = Quat.rotation(Vec3.axis_y(), 0.5 * math.JPH_PI * i) * Quat.rotation(Vec3.axis_x(), math.degrees_to_radians(1.0))
            belt_settings.position = belt_settings.rotation * Vec3(c_belt_length, 6.0, c_belt_width)
            belt_id = body_interface.create_and_add_body(belt_settings, pyjolt.EActivation.DONT_ACTIVATE)
            self.linear_belts.append(belt_id)

        # Bodies with decreasing friction
        cargo_shape = pyjolt.BoxShape(Vec3.replicate(2.0))
        cargo_settings = pyjolt.BodyCreationSettings(cargo_shape, Vec3.zero(), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        for i in range(11):
            cargo_settings.position = Vec3(-c_belt_length + i * 10.0, 10.0, -c_belt_length)
            cargo_settings.friction = max(0.0, 1.0 - 0.1 * i)
            body_interface.create_and_add_body(cargo_settings, pyjolt.EActivation.ACTIVATE)

        # Create 2 cylinders
        cylinder_settings = pyjolt.BodyCreationSettings(pyjolt.CylinderShape(6.0, 1.0), Vec3(-25.0, 1.0, -20.0),
                                                        Quat.rotation(Vec3.axis_z(), 0.5 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        body_interface.create_and_add_body(cylinder_settings, pyjolt.EActivation.ACTIVATE)
        cylinder_settings.position.z = 20.0
        body_interface.create_and_add_body(cylinder_settings, pyjolt.EActivation.ACTIVATE)

        # Let a dynamic belt rest on it
        dynamic_belt_settings = pyjolt.BodyCreationSettings(pyjolt.BoxShape(Vec3(5.0, 0.1, 25.0), 0.0), Vec3(-25.0, 3.0, 0), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        dynamic_belt_id = body_interface.create_and_add_body(dynamic_belt_settings, pyjolt.EActivation.ACTIVATE)
        self.linear_belts.append(dynamic_belt_id)

        # Create cargo on the dynamic belt
        cargo_settings.position = Vec3(-25.0, 6.0, 15.0)
        cargo_settings.friction = 1.0
        body_interface.create_and_add_body(cargo_settings, pyjolt.EActivation.ACTIVATE)

        # Create an angular belt
        angular_belt_settings = pyjolt.BodyCreationSettings(pyjolt.BoxShape(Vec3(20.0, 0.1, 20.0), 0.0), Vec3(10.0, 3.0, 0), Quat.identity(), pyjolt.EMotionType.STATIC, Layers.NON_MOVING)
        self.angular_belt = body_interface.create_and_add_body(angular_belt_settings, pyjolt.EActivation.ACTIVATE)

        # Bodies with decreasing friction dropping on the angular belt
        for i in range(7):
            cargo_settings.position = Vec3(10.0, 10.0, -15.0 + 5.0 * i)
            cargo_settings.friction = max(0.0, 1.0 - 0.1 * i)
            body_interface.create_and_add_body(cargo_settings, pyjolt.EActivation.ACTIVATE)

    def cleanup(self):
        self.physics_system.set_contact_listener(None)
        super().cleanup()