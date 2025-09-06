#include "Common.h"
#include <Jolt/Physics/Ragdoll/Ragdoll.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <nanobind/stl/pair.h>

void BindRagdoll(nb::module_ &m) {
    nb::class_<RagdollSettings, RefTarget<RagdollSettings>> ragdollSettingsCls(m, "RagdollSettings",
        nb::intrusive_ptr<RagdollSettings>([](RagdollSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Contains the structure of a ragdoll");
    ragdollSettingsCls
        .def("stabilize", &RagdollSettings::Stabilize,
            "Stabilize the constraints of the ragdoll\n"
            "@return True on success, false on failure.")
        .def("disable_parent_child_collisions", &RagdollSettings::DisableParentChildCollisions, "joint_matrices"_a = nullptr, "min_separation_distance"_a = 0.0f,
            "After the ragdoll has been fully configured, call this function to automatically create and add a GroupFilterTable collision filter to all bodies\n"
            "and configure them so that parent and children don't collide.\n"
            "This will:\n"
            "- Create a GroupFilterTable and assign it to all of the bodies in a ragdoll.\n"
            "- Each body in your ragdoll will get a SubGroupID that is equal to the joint index in the Skeleton that it is attached to.\n"
            "- Loop over all joints in the Skeleton and call GroupFilterTable::DisableCollision(joint index, parent joint index).\n"
            "- When a pose is provided through inJointMatrices the function will detect collisions between joints\n"
            "(they must be separated by more than inMinSeparationDistance to be treated as not colliding) and automatically disable collisions.\n"
            "When you create an instance using Ragdoll::CreateRagdoll pass in a unique GroupID for each ragdoll (e.g. a simple counter), note that this number\n"
            "should be unique throughout the PhysicsSystem, so if you have different types of ragdolls they should not share the same GroupID.")
        .def("save_binary_state", &RagdollSettings::SaveBinaryState, "stream"_a, "save_shapes"_a, "save_group_filter"_a,
            "Saves the state of this object in binary form to inStream.\n"
            "Args:\n"
            "    stream (StreamOut): The stream to save the state to.\n"
            "    save_shapes (bool): If the shapes should be saved as well (these could be shared between ragdolls, in which case the calling application may want to write custom code to restore them).\n"
            "    save_group_filter (bool): If the group filter should be saved as well (these could be shared).")
        .def_static("restore_from_binary_state", &RagdollSettings::sRestoreFromBinaryState, "stream"_a,
            "Restore a saved ragdoll from inStream")
        .def("create_ragdoll", &RagdollSettings::CreateRagdoll, "collision_group"_a, "user_data"_a, "system"_a, nb::rv_policy::reference,
            "Create ragdoll instance from these settings\n"
            "@return Newly created ragdoll or null when out of bodies")
        .def("get_skeleton", nb::overload_cast<>(&RagdollSettings::GetSkeleton), nb::rv_policy::reference,
            "Access to the skeleton of this ragdoll")
        // .def("get_skeleton", &RagdollSettings::GetSkeleton, nb::rv_policy::reference)
        .def("calculate_body_index_to_constraint_index", &RagdollSettings::CalculateBodyIndexToConstraintIndex,
            "Calculate the map needed for GetBodyIndexToConstraintIndex()")
        .def("get_body_index_to_constraint_index", &RagdollSettings::GetBodyIndexToConstraintIndex,
            "Get table that maps a body index to the constraint index with which it is connected to its parent. -1 if there is no constraint associated with the body.\n"
            "Note that this will only tell you which constraint connects the body to its parent, it will not look in the additional constraint list.")
        .def("get_constraint_index_for_body_index", &RagdollSettings::GetConstraintIndexForBodyIndex, "body_index"_a,
            "Map a single body index to a constraint index")
        .def("calculate_constraint_index_to_body_idx_pair", &RagdollSettings::CalculateConstraintIndexToBodyIdxPair,
            "Calculate the map needed for GetConstraintIndexToBodyIdxPair()")
        .def("get_constraint_index_to_body_idx_pair", &RagdollSettings::GetConstraintIndexToBodyIdxPair,
            "Table that maps a constraint index (index in mConstraints) to the indices of the bodies that the constraint is connected to (index in mBodyIDs)")
        .def("get_body_indices_for_constraint_index", &RagdollSettings::GetBodyIndicesForConstraintIndex, "constraint_index"_a,
            "Map a single constraint index (index in mConstraints) to the indices of the bodies that the constraint is connected to (index in mBodyIDs)")
        .def_rw("skeleton", &RagdollSettings::mSkeleton,
            "The skeleton for this ragdoll")
        .def_rw("parts", &RagdollSettings::mParts,
            "For each of the joints, the body and constraint attaching it to its parent body (1-on-1 with mSkeleton.GetJoints())")
        .def_rw("additional_constraints", &RagdollSettings::mAdditionalConstraints,
            "A list of constraints that connects two bodies in a ragdoll (for non parent child related constraints)");

    nb::class_<Ragdoll, RefTarget<Ragdoll>> ragdollCls(m, "Ragdoll",
        nb::intrusive_ptr<Ragdoll>([](Ragdoll *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Runtime ragdoll information");
    ragdollCls
        .def(nb::init<PhysicsSystem *>(), "system"_a,
            "Constructor")
        .def("add_to_physics_system", &Ragdoll::AddToPhysicsSystem, "activation_mode"_a, "lock_bodies"_a = true,
            "Add bodies and constraints to the system and optionally activate the bodies")
        .def("remove_from_physics_system", &Ragdoll::RemoveFromPhysicsSystem, "lock_bodies"_a = true,
            "Remove bodies and constraints from the system")
        .def("activate", &Ragdoll::Activate, "lock_bodies"_a = true,
            "Wake up all bodies in the ragdoll")
        .def("is_active", &Ragdoll::IsActive, "lock_bodies"_a = true,
            "Check if one or more of the bodies in the ragdoll are active.\n"
            "Note that this involves locking the bodies (if inLockBodies is true) and looping over them. An alternative and possibly faster\n"
            "way could be to install a BodyActivationListener and count the number of active bodies of a ragdoll as they're activated / deactivated\n"
            "(basically check if the body that activates / deactivates is in GetBodyIDs() and increment / decrement a counter).")
        .def("set_group_id", &Ragdoll::SetGroupID, "group_id"_a, "lock_bodies"_a = true,
            "Set the group ID on all bodies in the ragdoll")
        .def("set_pose", nb::overload_cast<const SkeletonPose &, bool>(&Ragdoll::SetPose), "pose"_a, "lock_bodies"_a = true,
            "Set the ragdoll to a pose (calls BodyInterface::SetPositionAndRotation to instantly move the ragdoll)")
        .def("set_pose", nb::overload_cast<RVec3Arg, const Mat44 *, bool>(&Ragdoll::SetPose), "root_offset"_a, "joint_matrices"_a, "lock_bodies"_a = true,
            "Lower level version of SetPose that directly takes the world space joint matrices")
        .def("get_pose", nb::overload_cast<SkeletonPose &, bool>(&Ragdoll::GetPose), "pose"_a, "lock_bodies"_a = true,
            "Get the ragdoll pose (uses the world transform of the bodies to calculate the pose)")
        .def("get_pose", nb::overload_cast<RVec3 &, Mat44 *, bool>(&Ragdoll::GetPose), "root_offset"_a, "joint_matrices"_a, "lock_bodies"_a = true,
            "Lower level version of GetPose that directly returns the world space joint matrices")
        .def("reset_warm_start", &Ragdoll::ResetWarmStart,
            "This function calls ResetWarmStart on all constraints. It can be used after calling SetPose to reset previous frames impulses. See: Constraint::ResetWarmStart.")
        .def("drive_to_pose_using_kinematics", nb::overload_cast<const SkeletonPose &, float , bool>(&Ragdoll::DriveToPoseUsingKinematics), "pose"_a, "delta_time"_a, "lock_bodies"_a = true,
            "Drive the ragdoll to a specific pose by setting velocities on each of the bodies so that it will reach inPose in inDeltaTime")
        .def("drive_to_pose_using_kinematics", nb::overload_cast<RVec3Arg, const Mat44 *, float , bool>(&Ragdoll::DriveToPoseUsingKinematics), "root_offset"_a, "joint_matrices"_a, "delta_time"_a, "lock_bodies"_a = true,
            "Lower level version of DriveToPoseUsingKinematics that directly takes the world space joint matrices")
        .def("drive_to_pose_using_motors", &Ragdoll::DriveToPoseUsingMotors, "pose"_a,
            "Drive the ragdoll to a specific pose by activating the motors on each constraint")
        .def("set_linear_and_angular_velocity", &Ragdoll::SetLinearAndAngularVelocity, "linear_velocity"_a, "angular_velocity"_a, "lock_bodies"_a = true,
            "Control the linear and velocity of all bodies in the ragdoll")
        .def("set_linear_velocity", &Ragdoll::SetLinearVelocity, "linear_velocity"_a, "lock_bodies"_a = true,
            "Set the world space linear velocity of all bodies in the ragdoll.")
        .def("add_linear_velocity", &Ragdoll::AddLinearVelocity, "linear_velocity"_a, "lock_bodies"_a = true,
            "Add a world space velocity (in m/s) to all bodies in the ragdoll.")
        .def("add_impulse", &Ragdoll::AddImpulse, "impulse"_a, "lock_bodies"_a = true,
            "Add impulse to all bodies of the ragdoll (center of mass of each of them)")
        .def("get_root_transform", &Ragdoll::GetRootTransform, "position"_a, "rotation"_a, "lock_bodies"_a = true,
            "Get the position and orientation of the root of the ragdoll")
        .def("get_body_count", &Ragdoll::GetBodyCount,
            "Get number of bodies in the ragdoll")
        .def("get_body_id", &Ragdoll::GetBodyID, "body_index"_a,
            "Access a body ID")
        .def("get_body_i_ds", &Ragdoll::GetBodyIDs,
            "Access to the array of body IDs")
        .def("get_constraint_count", &Ragdoll::GetConstraintCount,
            "Get number of constraints in the ragdoll")
        .def("get_constraint", nb::overload_cast<int>(&Ragdoll::GetConstraint), "constraint_index"_a, nb::rv_policy::reference,
            "Access a constraint by index")
        .def("get_world_space_bounds", &Ragdoll::GetWorldSpaceBounds, "lock_bodies"_a = true,
            "Get world space bounding box for all bodies of the ragdoll")
        .def("get_ragdoll_settings", &Ragdoll::GetRagdollSettings, nb::rv_policy::reference,
            "Get the settings object that created this ragdoll");

    nb::class_<RagdollSettings::Part, BodyCreationSettings> partCls(ragdollSettingsCls, "Part",
        "A single rigid body sub part of the ragdoll");
    partCls
        .def_rw("to_parent", &RagdollSettings::Part::mToParent);

    nb::class_<RagdollSettings::AdditionalConstraint> additionalConstraintCls(ragdollSettingsCls, "AdditionalConstraint",
        "A constraint that connects two bodies in a ragdoll (for non parent child related constraints)");
    additionalConstraintCls
        .def(nb::init<>(),
            "Constructors")
        .def(nb::init<int, int, TwoBodyConstraintSettings *>(), "body_idx1"_a, "body_idx2"_a, "constraint"_a)
        // .def_rw("body_idx", &RagdollSettings::AdditionalConstraint::mBodyIdx,
            // "Indices of the bodies that this constraint connects")                // TODO:
        .def_rw("constraint", &RagdollSettings::AdditionalConstraint::mConstraint,
            "The constraint that connects these bodies");
}
