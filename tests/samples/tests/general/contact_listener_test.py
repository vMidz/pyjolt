import pyjolt
from pyjolt import math
from pyjolt.math import Vec3, Quat
from tests.test_common import Test, Layers, Context
import threading
from collections import namedtuple
from pyjolt import Body, CollideShapeResult

PredictedVelocity = namedtuple("PredictedVelocity", ["body_id", "linear_velocity", "angular_velocity"])

class ContactListenerImpl(pyjolt.ContactListener):
    def __init__(self, test_instance: 'ContactListenerTest'):
        super().__init__()
        self.test_instance = test_instance

    def on_contact_validate(self, body1: Body, body2: Body, base_offset: Vec3, collision_result: CollideShapeResult) -> pyjolt.ValidateResult:
        # Body 1 and 2 should never collide
        body0_id = self.test_instance.bodies[0].get_id()
        body1_id = self.test_instance.bodies[1].get_id()

        if (body1.get_id() == body0_id and body2.get_id() == body1_id) or \
           (body1.get_id() == body1_id and body2.get_id() == body0_id):
            return pyjolt.ValidateResult.REJECT_ALL_CONTACTS_FOR_THIS_BODY_PAIR
        
        return pyjolt.ValidateResult.ACCEPT_ALL_CONTACTS_FOR_THIS_BODY_PAIR

    def on_contact_added(self, in_body1: Body, in_body2: Body, in_manifold: pyjolt.ContactManifold, io_settings: pyjolt.ContactSettings) -> None:
        body0_id = self.test_instance.bodies[0].get_id()
        if in_body1.get_id() == body0_id or in_body2.get_id() == body0_id:
            io_settings.combined_restitution = 1.0

        result = pyjolt.CollisionEstimationResult()
        pyjolt.estimate_collision_response(in_body1, in_body2, in_manifold, result, 
                                           io_settings.combined_friction, io_settings.combined_restitution)

        impulses_str = ""
        for impulse in result.impulses:
            impulses_str += f"({impulse.contact_impulse:.3f}, {impulse.friction_impulse1:.3f}, {impulse.friction_impulse2:.3f}) "

        pyjolt.trace(f"Estimated velocity after collision, body1: {in_body1.get_id().get_index()}, "
                     f"v={result.linear_velocity1}, w={result.angular_velocity1}, "
                     f"body2: {in_body2.get_id().get_index()}, v={result.linear_velocity2}, w={result.angular_velocity2}, "
                     f"impulses: {impulses_str}")

        # Log predicted velocities for checking in post_physics_update
        with self.test_instance.predicted_velocities_mutex:
            self.test_instance.predicted_velocities.append(PredictedVelocity(in_body1.get_id(), result.linear_velocity1, result.angular_velocity1))
            self.test_instance.predicted_velocities.append(PredictedVelocity(in_body1.get_id(), result.linear_velocity2, result.angular_velocity2))

    def on_contact_persisted(self, in_body1: Body, in_body2: Body, in_manifold: pyjolt.ContactManifold, io_settings: pyjolt.ContactSettings) -> None:
        return

    def on_contact_removed(self, in_sub_shape_pair: pyjolt.SubShapeIDPair) -> None:
        return

class ContactListenerTest(Test):
    def __init__(self, ctx: Context):
        super().__init__(ctx)
        body_interface = self.physics_system.get_body_interface()

        # Member variables
        self.bodies = []
        self.predicted_velocities = []
        self.predicted_velocities_mutex = threading.Lock()

        # Floor
        self.create_floor()

        box_shape = pyjolt.BoxShape(Vec3(0.5, 1.0, 2.0))

        # Dynamic body 1
        settings1 = pyjolt.BodyCreationSettings(box_shape, Vec3(0, 10, 0), Quat.identity(), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        body1 = body_interface.create_body(settings1)
        body1.allow_sleeping = False
        body_interface.add_body(body1.get_id(), pyjolt.EActivation.ACTIVATE)
        self.bodies.append(body1)

        # Dynamic body 2
        settings2 = pyjolt.BodyCreationSettings(box_shape, Vec3(5, 10, 0), Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        body2 = body_interface.create_body(settings2)
        body2.allow_sleeping = False
        body_interface.add_body(body2.get_id(), pyjolt.EActivation.ACTIVATE)
        self.bodies.append(body2)

        # Dynamic body 3
        settings3 = pyjolt.BodyCreationSettings(pyjolt.SphereShape(2.0), Vec3(10, 10, 0), Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        body3 = body_interface.create_body(settings3)
        body3.allow_sleeping = False
        body_interface.add_body(body3.get_id(), pyjolt.EActivation.ACTIVATE)
        self.bodies.append(body3)

        # Dynamic body 4
        compound_settings = pyjolt.StaticCompoundShapeSettings()
        compound_settings.add_shape(Vec3.zero(), Quat.identity(), pyjolt.CapsuleShape(5, 1))
        compound_settings.add_shape(Vec3(0, -5, 0), Quat.identity(), pyjolt.SphereShape(2))
        compound_settings.add_shape(Vec3(0, 5, 0), Quat.identity(), pyjolt.SphereShape(2))
        
        settings4 = pyjolt.BodyCreationSettings(compound_settings, Vec3(15, 10, 0), Quat.rotation(Vec3.axis_x(), 0.25 * math.JPH_PI), pyjolt.EMotionType.DYNAMIC, Layers.MOVING)
        body4 = body_interface.create_body(settings4)
        body4.allow_sleeping = False
        body_interface.add_body(body4.get_id(), pyjolt.EActivation.ACTIVATE)
        self.bodies.append(body4)

        # Set contact listener
        self.listener = ContactListenerImpl(self)
        self.physics_system.set_contact_listener(self.listener)

    # TODO:
    def post_physics_update(self, delta_time: float) -> None:
        self.predicted_velocities.clear()
        return 
        # Check that predicted velocities match actual velocities
        body_lock_interface = self.physics_system.get_body_lock_interface()

        with self.predicted_velocities_mutex:
            for pv in self.predicted_velocities:
                body_lock = pyjolt.BodyLockRead()
                if body_lock.succeeded():
                    body = body_lock.get_body()
                    linear_velocity = body.angular_velocity
                    angular_velocity = body.angular_velocity
                    
                    diff_v = (pv.linear_velocity - linear_velocity).length()
                    diff_w = (pv.angular_velocity - angular_velocity).length()

                    if diff_v > 1.0e-3 or diff_w > 1.0e-3:
                        pyjolt.trace(f"Mispredicted collision for body: {body.get_id().get_index()}, "
                                        f"v={linear_velocity}, w={angular_velocity}, "
                                        f"predicted_v={pv.linear_velocity}, predicted_w={pv.angular_velocity}, "
                                        f"diff_v={diff_v}, diff_w={diff_w}")
        
            
            self.predicted_velocities.clear()

    def cleanup(self):
        # Unset listener before cleaning up bodies and test environment
        self.physics_system.set_contact_listener(None)
        super().cleanup()