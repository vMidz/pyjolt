from tests.test_common import Test, Layers, Context
import pyjolt
from pyjolt import Body, ContactManifold, ContactSettings, StateRecorder
from pyjolt.math import Vec3, Quat

class ContactListenerImpl(pyjolt.ContactListener):
    def __init__(self, test_instance: 'ModifyMassTest'):
        super().__init__()
        self.test_instance = test_instance

    def on_contact_added(self, body1: Body, body2: Body, manifold: ContactManifold, settings: ContactSettings) -> None:
            # We're only concerned with dynamic bodies (floor gets normal collision response)
            if not body1.is_dynamic() or not body2.is_dynamic():
                return

            # Override the mass of body 1
            scale1 = self.test_instance.s_get_inv_mass_scale(body1)
            settings.inv_mass_scale1 = scale1
            settings.inv_inertia_scale1 = scale1

            # Override the mass of body 2
            scale2 = self.test_instance.s_get_inv_mass_scale(body2)
            settings.inv_mass_scale2 = scale2
            settings.inv_inertia_scale2 = scale2

    def on_contact_persisted(self, body1: Body, body2: Body, manifold: ContactManifold, settings: ContactSettings) -> None:
        self.on_contact_added(body1, body2, manifold, settings)

class ModifyMassTest(Test):
    @staticmethod
    def s_get_inv_mass_scale(in_body: Body) -> float:
        ud = in_body.user_data
        # In the C++ code, the cycle number for the left body (index 0) is stored in bits 3-x
        # and for the right body (index 1) in bits 1-x.
        # This logic decodes that to get the index into the mass_overrides array.
        if (ud & 1) != 0: # Body 2
            index = (ud >> 1) & 0b11
        else: # Body 1
            index = (ud >> 3) & 0b11
            
        mass_overrides = [1.0, 0.0, 0.5, 2.0]
        return mass_overrides[index]

    def __init__(self, ctx: Context):
        super().__init__(ctx)
        
        self.contact_listener = ContactListenerImpl(self)
        self.physics_system.set_contact_listener(self.contact_listener)

        # Member variables
        self.time = 0.0
        self.bodies = [None, None]
        body_interface = self.physics_system.get_body_interface()

        # Floor
        self.create_floor()

        # Create two spheres on a collision course
        bcs = pyjolt.BodyCreationSettings(
            pyjolt.SphereShape(1.0), 
            Vec3.zero(), 
            Quat.identity(), 
            pyjolt.EMotionType.DYNAMIC, 
            Layers.MOVING
        )
        bcs.restitution = 1.0
        self.bodies[0] = body_interface.create_and_add_body(bcs, pyjolt.EActivation.ACTIVATE)
        self.bodies[1] = body_interface.create_and_add_body(bcs, pyjolt.EActivation.ACTIVATE)
        
        self.reset_bodies(0)

    def reset_bodies(self, in_cycle: int):
        body_interface = self.physics_system.get_body_interface()

        # Left body
        body_interface.set_position_and_rotation(self.bodies[0], Vec3(-5, 5, 0), Quat.identity(), pyjolt.EActivation.ACTIVATE)
        body_interface.set_linear_and_angular_velocity(self.bodies[0], Vec3(10, 0, 0), Vec3.zero())
        # Store cycle in bits 3-x so we can have 4 different mass settings for this body
        body_interface.set_user_data(self.bodies[0], (in_cycle << 3))

        # Right body
        body_interface.set_position_and_rotation(self.bodies[1], Vec3(5, 5, 0), Quat.identity(), pyjolt.EActivation.ACTIVATE)
        body_interface.set_linear_and_angular_velocity(self.bodies[1], Vec3(-10, 0, 0), Vec3.zero())
        # Store cycle in bits 1-x and set bit 0 to identify the body
        body_interface.set_user_data(self.bodies[1], (in_cycle << 1) + 1)

    def pre_physics_update(self, delta_time) -> None:
        c_time_between_tests = 2.0

        old_cycle = int(self.time / c_time_between_tests)
        self.time += delta_time
        new_cycle = int(self.time / c_time_between_tests)
        
        if old_cycle != new_cycle:
            self.reset_bodies(new_cycle)
    
    def post_physics_update(self, delta_time: float) -> None:
        ...
        # TODO:
        # Draw the mass scale
        # body_lock_interface = self.physics_system.get_body_lock_interface()
        # for body_id in self.bodies:
        #     body_lock = pyjolt.BodyLockRead(body_lock_interface, body_id)
        #     if body_lock.succeeded():
        #         body = body_lock.get_body()
        #         inv_mass_scale = self.s_get_inv_mass_scale(body)
        #         velocity_x = body.linear_velocity.x
        #         text = f"Inv mass scale: {inv_mass_scale:.1f}\nVelocity X: {velocity_x:.1f}"
        #         pyjolt.DebugRenderer.instance.draw_text3d(body.get_position(), text, pyjolt.Color.White)

    def save_state(self, out_stream: StateRecorder) -> None:
        super().save_state(out_stream)
        out_stream.write_float(self.time)

    def restore_state(self, in_stream: StateRecorder) -> None:
        super().restore_state(in_stream)
        self.time = in_stream.read_float()

    def cleanup(self):
        self.physics_system.set_contact_listener(None)
        super().cleanup()