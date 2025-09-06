#include "Common.h"
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Body/Body.h>

#include <nanobind/ndarray.h>
#include <nanobind/trampoline.h>
#include <nanobind/eval.h>

using NumpyVec3Array = nb::ndarray<nb::numpy, float, nb::shape<-1, 3>>;

class PyContactListener : public ContactListener {

    NB_TRAMPOLINE(ContactListener, 4);

    ~PyContactListener() override {
    }

    ValidateResult OnContactValidate(
        const Body &inBody1,
        const Body &inBody2,
        RVec3Arg inBaseOffset,
        const CollideShapeResult &inCollisionResult) override {

        nb::handle self_py_handle = nb_trampoline.base();
        nanobind::detail::ticket nb_ticket(nb_trampoline, "on_contact_validate", false);
        static constexpr auto MSG = "Function: on_contact_validate not found, verify signature\n";
        if (nb_ticket.key.is_valid()) {
            nb::gil_scoped_acquire gil;
            try {
                // Create Python object
                nb::object py_body1 = nb::cast(const_cast<Body *>(&inBody1), nb::rv_policy::reference_internal, self_py_handle);
                nb::object py_body2 = nb::cast(const_cast<Body *>(&inBody2), nb::rv_policy::reference_internal, self_py_handle);
                nb::object py_base_offset = nb::cast(inBaseOffset, nb::rv_policy::automatic_reference);
                nb::object py_collision_result = nb::cast(inCollisionResult, nb::rv_policy::automatic_reference);
                // returns Python callable Object
                nb::object py_return_value = self_py_handle.attr(nb_ticket.key)(py_body1, py_body2, py_base_offset, py_collision_result);

                using nb_ret_type = decltype(NBBase::OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult));
                // Cast to Python
                return nb::cast<nb_ret_type>(py_return_value);
            } catch (nb::python_error &e) {
                e.restore();
                return NBBase::OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);
            } catch (...) {
                printf(MSG);
                return NBBase::OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);
            }
        } else {
            return NBBase::OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);
        }
    }

    void OnContactAdded(
        const Body &inBody1,
        const Body &inBody2,
        const ContactManifold &inManifold,
        ContactSettings &ioSettings) override { // Note: ioSettings is non-const

        nb::handle self_py_handle = nb_trampoline.base();
        nanobind::detail::ticket nb_ticket(nb_trampoline, "on_contact_added", false);
        static constexpr auto MSG = "Function: on_contact_added not found, verify signature\n";
        if (nb_ticket.key.is_valid()) {
            nb::gil_scoped_acquire gil;
            try {
                nb::object py_body1 = nb::cast(const_cast<Body *>(&inBody1), nb::rv_policy::reference_internal, self_py_handle);
                nb::object py_body2 = nb::cast(const_cast<Body *>(&inBody2), nb::rv_policy::reference_internal, self_py_handle);

                nb::object py_manifold = nb::cast(inManifold, nb::rv_policy::automatic_reference);

                nb::object py_settings = nb::cast(&ioSettings, nb::rv_policy::reference, self_py_handle);

                self_py_handle.attr(nb_ticket.key)(py_body1, py_body2, py_manifold, py_settings);
            } catch (nb::python_error &e) {
                e.restore();
                NBBase::OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
            } catch (...) {
                printf(MSG);
                NBBase::OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
            }
        } else {
            NBBase::OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
        }
    }

    void OnContactPersisted(
        const Body &inBody1,
        const Body &inBody2,
        const ContactManifold &inManifold,
        ContactSettings &ioSettings) override { // Note: ioSettings is non-const

        nb::handle self_py_handle = nb_trampoline.base();
        nanobind::detail::ticket nb_ticket(nb_trampoline, "on_contact_persisted", false);
        static constexpr auto MSG = "Function: on_contact_persisted not found, verify signature\n";
        if (nb_ticket.key.is_valid()) {
            nb::gil_scoped_acquire gil;
            try {
                nb::object py_body1 = nb::cast(const_cast<Body *>(&inBody1), nb::rv_policy::reference_internal, self_py_handle);
                nb::object py_body2 = nb::cast(const_cast<Body *>(&inBody2), nb::rv_policy::reference_internal, self_py_handle);
                nb::object py_manifold = nb::cast(inManifold, nb::rv_policy::automatic_reference);
                nb::object py_settings = nb::cast(&ioSettings, nb::rv_policy::reference, self_py_handle);

                self_py_handle.attr(nb_ticket.key)(py_body1, py_body2, py_manifold, py_settings);
            } catch (nb::python_error &e) {
                e.restore();
                NBBase::OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
            } catch (...) {
                printf(MSG);
                NBBase::OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
            }
        } else {
            NBBase::OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
        }
    }

    virtual void OnContactRemoved([[maybe_unused]] const SubShapeIDPair &inSubShapePair) override {
        NB_OVERRIDE_NAME(
            "on_contact_removed",
            OnContactRemoved,
            inSubShapePair);
    }
};

void BindContactListener(nb::module_ &m) {
    nb::class_<ContactManifold> contactManifoldCls(m, "ContactManifold",
        "Manifold class, describes the contact surface between two bodies");
    contactManifoldCls.def(nb::init<>())
        .def_rw("base_offset", &ContactManifold::mBaseOffset, "Offset to which all the contact points are relative")
        .def_rw("world_space_normal", &ContactManifold::mWorldSpaceNormal, "Normal for this manifold, direction along which to move body 2 out of collision along the shortest path")
        .def_rw("penetration_depth", &ContactManifold::mPenetrationDepth, "Penetration depth (move shape 2 by this distance to resolve the collision). If this value is negative, this is a speculative contact point and may not actually result in a velocity change as during solving the bodies may not actually collide.")
        .def_rw("sub_shape_id1", &ContactManifold::mSubShapeID1, "Sub shapes that formed this manifold (note that when multiple manifolds are combined because they're coplanar, we lose some information here because we only keep track of one sub shape pair that we encounter, see description at Body::SetUseManifoldReduction)")
        .def_rw("sub_shape_id2", &ContactManifold::mSubShapeID2, "Sub shape ID for body 2.")
        .def_prop_ro("relative_contact_points_on1", [](const ContactManifold &self) {
            const auto &points = self.mRelativeContactPointsOn1;
            size_t num_points = points.size();
            return NumpyVec3Array(
                (void *)points.data(),
                {num_points, 3},
                nb::find(&self));
        }, "Contact points on the surface of shape 1 relative to mBaseOffset.")
        .def_prop_ro("relative_contact_points_on2", [](const ContactManifold &self) {
            const auto &points = self.mRelativeContactPointsOn2;
            size_t num_points = points.size();
            return NumpyVec3Array(
                (void *)points.data(),
                {num_points, 3},
                nb::find(&self));
        }, "Contact points on the surface of shape 2 relative to mBaseOffset. If there's no penetration, this will be the same as mRelativeContactPointsOn1. If there is penetration they will be different.")
        .def("swap_shapes", &ContactManifold::SwapShapes, "Returns a new manifold with shape 1 and 2 swapped.")
        .def("get_world_space_contact_point_on1", &ContactManifold::GetWorldSpaceContactPointOn1,
            "index"_a, "Access to the world space contact positions")
        .def("get_world_space_contact_point_on2", &ContactManifold::GetWorldSpaceContactPointOn2,
            "index"_a, "Access to the world space contact positions");

    nb::class_<ContactSettings> contactSettingsCls(m, "ContactSettings", "Allows overriding contact properties in ContactListener callbacks.");
    contactSettingsCls
        .def(nb::init<>())
        .def_rw("combined_friction", &ContactSettings::mCombinedFriction,
            "Combined friction for the body pair (see: PhysicsSystem::SetCombineFriction)")
        .def_rw("combined_restitution", &ContactSettings::mCombinedRestitution,
            "Combined restitution for the body pair (see: PhysicsSystem::SetCombineRestitution)")
        .def_rw("inv_mass_scale1", &ContactSettings::mInvMassScale1,
            "Scale factor for the inverse mass of body 1 (0 = infinite mass, 1 = use original mass, 2 = body has half the mass). For the same contact pair, you should strive to keep the value the same over time.")
        .def_rw("inv_inertia_scale1", &ContactSettings::mInvInertiaScale1,
            "Scale factor for the inverse inertia of body 1 (usually same as mInvMassScale1)")
        .def_rw("inv_mass_scale2", &ContactSettings::mInvMassScale2,
            "Scale factor for the inverse mass of body 2 (0 = infinite mass, 1 = use original mass, 2 = body has half the mass). For the same contact pair, you should strive to keep the value the same over time.")
        .def_rw("inv_inertia_scale2", &ContactSettings::mInvInertiaScale2,
            "Scale factor for the inverse inertia of body 2 (usually same as mInvMassScale2)")
        .def_rw("is_sensor", &ContactSettings::mIsSensor, "Treat contact as sensor (no collision response).")
        .def_rw("relative_linear_surface_velocity", &ContactSettings::mRelativeLinearSurfaceVelocity,
            "Relative linear surface velocity between the bodies (world space surface velocity of body 2 - world space surface velocity of body 1), can be used to create a conveyor belt effect")
        .def_rw("relative_angular_surface_velocity", &ContactSettings::mRelativeAngularSurfaceVelocity,
            "Relative angular surface velocity between the bodies (world space angular surface velocity of body 2 - world space angular surface velocity of body 1). Note that this angular velocity is relative to the center of mass of body 1, so if you want it relative to body 2's center of mass you need to add body 2 angular velocity x (body 1 world space center of mass - body 2 world space center of mass) to mRelativeLinearSurfaceVelocity.");

    nb::enum_<ValidateResult>(m, "ValidateResult",
        "Return value for the OnContactValidate callback. Determines if the contact is being processed or not.\n"
        "Results are ordered so that the strongest accept has the lowest number and the strongest reject the highest number (which allows for easy combining of results)")
        .value("ACCEPT_ALL_CONTACTS_FOR_THIS_BODY_PAIR", ValidateResult::AcceptAllContactsForThisBodyPair,
            "Accept this and any further contact points for this body pair")
        .value("ACCEPT_CONTACT", ValidateResult::AcceptContact,
            "Accept this contact only (and continue calling this callback for every contact manifold for the same body pair)")
        .value("REJECT_CONTACT", ValidateResult::RejectContact,
            "Reject this contact only (but process any other contact manifolds for the same body pair)")
        .value("REJECT_ALL_CONTACTS_FOR_THIS_BODY_PAIR", ValidateResult::RejectAllContactsForThisBodyPair,
            "Rejects this and any further contact points for this body pair");

    nb::class_<ContactListener, PyContactListener>(m, "ContactListener",
        "A listener class that receives collision contact events.\n"
        "It can be registered with the ContactConstraintManager (or PhysicsSystem).\n"
        "Note that contact listener callbacks are called from multiple threads at the same time when all bodies are locked, you're only allowed to read from the bodies and you can't change physics state.\n"
        "During OnContactRemoved you cannot access the bodies at all, see the comments at that function.")
        .def(nb::init<>())
        .def("on_contact_validate", &ContactListener::OnContactValidate,
            "body1"_a, "body2"_a, "base_offset"_a, "collision_result"_a, nb::rv_policy::reference_internal, "Called after detecting a collision between a body pair, but before calling OnContactAdded and before adding the contact constraint.\n"
            "If the function rejects the contact, the contact will not be added and any other contacts between this body pair will not be processed.\n"
            "This function will only be called once per PhysicsSystem::Update per body pair and may not be called again the next update\n"
            "if a contact persists and no new contact pairs between sub shapes are found.\n"
            "This is a rather expensive time to reject a contact point since a lot of the collision detection has happened already, make sure you\n"
            "filter out the majority of undesired body pairs through the ObjectLayerPairFilter that is registered on the PhysicsSystem.\n"
            "Note that this callback is called when all bodies are locked, so don't use any locking functions!\n"
            "Body 1 will have a motion type that is larger or equal than body 2's motion type (order from large to small: dynamic -> kinematic -> static). When motion types are equal, they are ordered by BodyID.\n"
            "The collision result (inCollisionResult) is reported relative to inBaseOffset.")

        .def("on_contact_added", &ContactListener::OnContactAdded,
            "body1"_a, "body2"_a, "manifold"_a, "settings"_a, nb::rv_policy::reference_internal, "Called whenever a new contact point is detected.\n"
            "Note that this callback is called when all bodies are locked, so don't use any locking functions!\n"
            "Body 1 and 2 will be sorted such that body 1 ID < body 2 ID, so body 1 may not be dynamic.\n"
            "Note that only active bodies will report contacts, as soon as a body goes to sleep the contacts between that body and all other\n"
            "bodies will receive an OnContactRemoved callback, if this is the case then Body::IsActive() will return false during the callback.\n"
            "When contacts are added, the constraint solver has not run yet, so the collision impulse is unknown at that point.\n"
            "The velocities of inBody1 and inBody2 are the velocities before the contact has been resolved, so you can use this to\n"
            "estimate the collision impulse to e.g. determine the volume of the impact sound to play (see: EstimateCollisionResponse).")

        .def("on_contact_persisted", &ContactListener::OnContactPersisted,
            "body1"_a, "body2"_a, "manifold"_a, "settings"_a, nb::rv_policy::reference_internal, "Called whenever a contact is detected that was also detected last update.\n"
            "Note that this callback is called when all bodies are locked, so don't use any locking functions!\n"
            "Body 1 and 2 will be sorted such that body 1 ID < body 2 ID, so body 1 may not be dynamic.\n"
            "If the structure of the shape of a body changes between simulation steps (e.g. by adding/removing a child shape of a compound shape),\n"
            "it is possible that the same sub shape ID used to identify the removed child shape is now reused for a different child shape. The physics\n"
            "system cannot detect this, so may send a 'contact persisted' callback even though the contact is now on a different child shape. You can\n"
            "detect this by keeping the old shape (before adding/removing a part) around until the next PhysicsSystem::Update (when the OnContactPersisted\n"
            "callbacks are triggered) and resolving the sub shape ID against both the old and new shape to see if they still refer to the same child shape.")

        .def("on_contact_removed", &ContactListener::OnContactRemoved,
            "sub_shape_pair"_a, nb::rv_policy::reference_internal, "Called whenever a contact was detected last update but is not detected anymore.\n"
            "You cannot access the bodies at the time of this callback because:\n"
            "- All bodies are locked at the time of this callback.\n"
            "- Some properties of the bodies are being modified from another thread at the same time.\n"
            "- The body may have been removed and destroyed (you'll receive an OnContactRemoved callback in the PhysicsSystem::Update after the body has been removed).\n"
            "Cache what you need in the OnContactAdded and OnContactPersisted callbacks and store it in a separate structure to use during this callback.\n"
            "Alternatively, you could just record that the contact was removed and process it after PhysicsSimulation::Update.\n"
            "Body 1 and 2 will be sorted such that body 1 ID < body 2 ID, so body 1 may not be dynamic.\n"
            "The sub shape ID were created in the previous simulation step too, so if the structure of a shape changes (e.g. by adding/removing a child shape of a compound shape),\n"
            "the sub shape ID may not be valid / may not point to the same sub shape anymore.\n"
            "If you want to know if this is the last contact between the two bodies, use PhysicsSystem::WereBodiesInContact.");
}