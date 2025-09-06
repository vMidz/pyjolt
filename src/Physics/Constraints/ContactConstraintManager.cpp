#include "Common.h"
#include <Jolt/Physics/Constraints/ContactConstraintManager.h>
#include <Jolt/Physics/PhysicsUpdateContext.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Constraints/Constraint.h>

void BindContactConstraintManager(nb::module_ &m) {
    // TODO:
    nb::class_<ContactConstraintManager, NonCopyable> contactConstraintManagerCls(m, "ContactConstraintManager");
    contactConstraintManagerCls
        .def(nb::init<const PhysicsSettings &>(), "physics_settings"_a,
            "Constructor")
        .def("init", &ContactConstraintManager::Init, "max_body_pairs"_a, "max_contact_constraints"_a,
            "Initialize the system.\n"
            "Args:\n"
            "    max_body_pairs (int): Maximum amount of body pairs to process (anything else will fall through the world), this number should generally be much higher than the max amount of contact points as there will be lots of bodies close that are not actually touching.\n"
            "    max_contact_constraints (int): Maximum amount of contact constraints to process (anything else will fall through the world).")
        // .def("set_contact_listener", &ContactConstraintManager::SetContactListener, "listener"_a,
            // "Listener that is notified whenever a contact point between two bodies is added/updated/removed")
        // .def("get_contact_listener", &ContactConstraintManager::GetContactListener, nb::rv_policy::reference)
        // .def("set_combine_friction", &ContactConstraintManager::SetCombineFriction, "combine_friction"_a,
            // "Set the function that combines the friction of two bodies and returns it\n"
            // "Default method is the geometric mean: sqrt(friction1 * friction2).")
        // .def("get_combine_friction", &ContactConstraintManager::GetCombineFriction)
        // .def("set_combine_restitution", &ContactConstraintManager::SetCombineRestitution, "combine_restitution"_a,
            // "Set the function that combines the restitution of two bodies and returns it\n"
            // "Default method is max(restitution1, restitution1)")
        .def("get_combine_restitution", &ContactConstraintManager::GetCombineRestitution)
        .def("get_max_constraints", &ContactConstraintManager::GetMaxConstraints,
            "Get the max number of contact constraints that are allowed")
        .def("validate_contact_point", &ContactConstraintManager::ValidateContactPoint, "body1"_a, "body2"_a, "base_offset"_a, "collision_result"_a,
            "Check with the listener if inBody1 and inBody2 could collide, returns false if not")
        .def("prepare_constraint_buffer", &ContactConstraintManager::PrepareConstraintBuffer, "context"_a,
            "Sets up the constraint buffer. Should be called before starting collision detection.")
        .def("get_contact_allocator", &ContactConstraintManager::GetContactAllocator,
            "Get a new allocator context for storing contacts. Note that you should call this once and then add multiple contacts using the context.")
        .def("get_contacts_from_cache", [](ContactConstraintManager &self, ContactConstraintManager::ContactAllocator &ioContactAllocator, Body &inBody1, Body &inBody2) {
            bool pair_handled = false;
            bool constraint_created = false;
            self.GetContactsFromCache(ioContactAllocator, inBody1, inBody2, pair_handled, constraint_created);
            return nb::make_tuple(pair_handled, constraint_created);
        }, "contact_allocator"_a, "body1"_a, "body2"_a,
            "Check if the contact points from the previous frame are reusable and if so copy them.\n"
            "When the cache was usable and the pair has been handled: outPairHandled = true.\n"
            "When a contact constraint was produced: outConstraintCreated = true.\n"
            "Output: [pair_handled, constraint_created]")
        .def("add_body_pair", &ContactConstraintManager::AddBodyPair, "contact_allocator"_a, "body1"_a, "body2"_a,
            "Create a handle for a colliding body pair so that contact constraints can be added between them.\n"
            "Needs to be called once per body pair per frame before calling AddContactConstraint.")
        .def("add_contact_constraint", &ContactConstraintManager::AddContactConstraint,
            "contact_allocator"_a, "body_pair"_a, "body1"_a, "body2"_a, "manifold"_a,
            "Add a contact constraint for this frame.\n"
            "Args:\n"
            "    contact_allocator (ContactAllocator): The allocator that reserves memory for the contacts.\n"
            "    body_pair (BodyPairHandle): The handle for the contact cache for this body pair.\n"
            "    body1 (Body): The first body that is colliding.\n"
            "    body2 (Body): The second body that is colliding.\n"
            "    manifold (ContactManifold): The manifold that describes the collision.\n"
            "Returns:\n"
            "    bool: true if a contact constraint was created (can be false in the case of a sensor)\n"
            "This is using the approach described in 'Modeling and Solving Constraints' by Erin Catto presented at GDC 2009 (and later years with slight modifications).\n"
            "We're using the formulas from slide 50 - 53 combined.\n"
            "Euler velocity integration:\n"
            "v1' = v1 + M^-1 P\n"
            "Impulse:\n"
            "P = J^T lambda\n"
            "Constraint force:\n"
            "lambda = -K^-1 J v1\n"
            "Inverse effective mass:\n"
            "K = J M^-1 J^T\n"
            "Constraint equation (limits movement in 1 axis):\n"
            "C = (p2 - p1) . n\n"
            "Jacobian (for position constraint)\n"
            "J = [-n, -r1 x n, n, r2 x n]\n"
            "n = contact normal (pointing away from body 1).\n"
            "p1, p2 = positions of collision on body 1 and 2.\n"
            "r1, r2 = contact point relative to center of mass of body 1 and body 2 (r1 = p1 - x1, r2 = p2 - x2).\n"
            "v1, v2 = (linear velocity, angular velocity): 6 vectors containing linear and angular velocity for body 1 and 2.\n"
            "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].")
        .def("finalize_contact_cache_and_call_contact_point_removed_callbacks", &ContactConstraintManager::FinalizeContactCacheAndCallContactPointRemovedCallbacks, "expected_num_body_pairs"_a, "expected_num_manifolds"_a,
            "Finalizes the contact cache, the contact cache that was generated during the calls to AddContactConstraint in this update\n"
            "will be used from now on to read from. After finalizing the contact cache, the contact removed callbacks will be called.\n"
            "inExpectedNumBodyPairs / inExpectedNumManifolds are the amount of body pairs / manifolds found in the previous step and is\n"
            "used to determine the amount of buckets the contact cache hash map will use in the next update.")
        // .def("were_bodies_in_contact", &ContactConstraintManager::WereBodiesInContact, "body1_id"_a, "body2_id"_a,
            // "Check if 2 bodies were in contact during the last simulation step. Since contacts are only detected between active bodies, at least one of the bodies must be active.\n"
            // "Uses the read collision cache to determine if 2 bodies are in contact.")
        .def("get_num_constraints", &ContactConstraintManager::GetNumConstraints,
                "Get the number of contact constraints that were found")
        .def("sort_contacts", [](ContactConstraintManager &self, Array<uint32 > constraintArray) {
            if (constraintArray.empty())
                throw nb::type_error("Input array is empty");
            self.SortContacts(constraintArray.data(), constraintArray.data() + constraintArray.size());
        }, "constraint_array"_a, "Sort contact constraints deterministically")
        // .def("get_affected_bodies", &ContactConstraintManager::GetAffectedBodies, "constraint_idx"_a, "body1"_a, "body2"_a,
            // "Get the affected bodies for a given constraint")
        // .def("warm_start_velocity_constraints", &ContactConstraintManager::WarmStartVelocityConstraints, "constraint_idx_begin"_a, "constraint_idx_end"_a, "warm_start_impulse_ratio"_a, "callback"_a,
            // "Apply last frame's impulses as an initial guess for this frame's impulses")
        .def("solve_velocity_constraints", &ContactConstraintManager::SolveVelocityConstraints, "constraint_idx_begin"_a, "constraint_idx_end"_a,
            "Solve velocity constraints, when almost nothing changes this should only apply very small impulses\n"
            "since we're warm starting with the total impulse applied in the last frame above.\n"
            "Friction wise we're using the Coulomb friction model which says that:\n"
            "|F_T| <= mu |F_N|\n"
            "Where F_T is the tangential force, F_N is the normal force and mu is the friction coefficient\n"
            "In impulse terms this becomes:\n"
            "|lambda_T| <= mu |lambda_N|\n"
            "And the constraint that needs to be applied is exactly the same as a non penetration constraint\n"
            "except that we use a tangent instead of a normal. The tangent should point in the direction of the\n"
            "tangential velocity of the point:\n"
            "J = [-T, -r1 x T, T, r2 x T]\n"
            "Where T is the tangent.\n"
            "See slide 42 and 43.\n"
            "Restitution is implemented as a velocity bias (see slide 41):\n"
            "b = e v_n^-\n"
            "e = the restitution coefficient, v_n^- is the normal velocity prior to the collision\n"
            "Restitution is only applied when v_n^- is large enough and the points are moving towards collision")
        // .def("store_applied_impulses", &ContactConstraintManager::StoreAppliedImpulses, "constraint_idx_begin"_a, "constraint_idx_end"_a,
        //     "Save back the lambdas to the contact cache for the next warm start")
        // .def("solve_position_constraints", &ContactConstraintManager::SolvePositionConstraints, "constraint_idx_begin"_a, "constraint_idx_end"_a,
        //     "Solve position constraints.\n"
        //     "This is using the approach described in 'Modeling and Solving Constraints' by Erin Catto presented at GDC 2007.\n"
        //     "On slide 78 it is suggested to split up the Baumgarte stabilization for positional drift so that it does not\n"
        //     "actually add to the momentum. We combine an Euler velocity integrate + a position integrate and then discard the velocity\n"
        //     "change.\n"
        //     "Constraint force:\n"
        //     "lambda = -K^-1 b\n"
        //     "Baumgarte stabilization:\n"
        //     "b = beta / dt C\n"
        //     "beta = baumgarte stabilization factor.\n"
        //     "dt = delta time.")
        .def("recycle_constraint_buffer", &ContactConstraintManager::RecycleConstraintBuffer,
            "Recycle the constraint buffer. Should be called between collision simulation steps.")
        .def("finish_constraint_buffer", &ContactConstraintManager::FinishConstraintBuffer,
            "Terminate the constraint buffer. Should be called after simulation ends.")
        .def("on_ccd_contact_added", &ContactConstraintManager::OnCCDContactAdded, "contact_allocator"_a, "body1"_a, "body2"_a, "manifold"_a, "settings"_a,
            "Called by continuous collision detection to notify the contact listener that a contact was added.\n"
            "Args:\n"
            "    contact_allocator (ContactAllocator): The allocator that reserves memory for the contacts.\n"
            "    body1 (Body): The first body that is colliding.\n"
            "    body2 (Body): The second body that is colliding.\n"
            "    manifold (ContactManifold): The manifold that describes the collision.\n"
            "    settings (ContactSettings): The calculated contact settings (may be overridden by the contact listener).")
        .def("save_state", &ContactConstraintManager::SaveState, "stream"_a, "filter"_a,
            "Saving state for replay")
        .def("restore_state", &ContactConstraintManager::RestoreState, "stream"_a, "filter"_a,
            "Restoring state for replay. Returns false when failed.")
        .def_ro_static("MAX_CONTACT_POINTS", &ContactConstraintManager::MaxContactPoints,
            "Max 4 contact points are needed for a stable manifold")
        .def_rw_static("draw_contact_point", &ContactConstraintManager::sDrawContactPoint)
        .def_rw_static("draw_supporting_faces", &ContactConstraintManager::sDrawSupportingFaces)
        .def_rw_static("draw_contact_point_reduction", &ContactConstraintManager::sDrawContactPointReduction)
        .def_rw_static("draw_contact_manifolds", &ContactConstraintManager::sDrawContactManifolds);

    nb::class_<ContactConstraintManager::ContactAllocator, LFHMAllocatorContext> contactAllocatorCls(contactConstraintManagerCls, "ContactAllocator",
            "Contacts are allocated in a lock free hash map");
    contactAllocatorCls
        .def_rw("num_body_pairs", &ContactConstraintManager::ContactAllocator::mNumBodyPairs,
            "Total number of body pairs added using this allocator")
        .def_rw("num_manifolds", &ContactConstraintManager::ContactAllocator::mNumManifolds,
            "Total number of manifolds added using this allocator")
        .def_rw("errors", &ContactConstraintManager::ContactAllocator::mErrors,
            "Errors reported on this allocator");

}