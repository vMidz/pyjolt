#include "Common.h"
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>

void BindBody(nb::module_ &m) {
    nb::class_<Body>(m, "Body",
        "A rigid body that can be simulated using the physics system\n"
        "Note that internally all properties (position, velocity etc.) are tracked relative to the center of mass of the object to simplify the simulatio\n"
        "The offset between the position of the body and the center of mass position of the body is GetShape()->GetCenterOfMass().\n"
        "The functions that get/set the position of the body all indicate if they are relative to the center of mass or to the original position in which\n"
        "The linear velocity is also velocity of the center of mass, to correct for this: \\f$VelocityCOM = Velocity - AngularVelocity \\times ShapeCOM\\f$.")

        .def("get_id", &Body::GetID, "Get the id of this body")
        .def("get_body_type", &Body::GetBodyType, "Get the type of body (rigid or soft)")
        .def("is_rigid_body", &Body::IsRigidBody, "Check if this body is a rigid body")
        .def("is_soft_body", &Body::IsSoftBody, "Check if this body is a soft body")
        .def("is_active", &Body::IsActive, "If this body is currently actively simulating (true) or sleeping (false)")
        .def("is_static", &Body::IsStatic, "Check if this body is static (not movable)")
        .def("is_kinematic", &Body::IsKinematic,
            "Check if this body is kinematic (keyframed), which means that it will move according to its current velocity, but forces don't affect it")
        .def("is_dynamic", &Body::IsDynamic, "Check if this body is dynamic, which means that it moves and forces can act on it")
        .def("can_be_kinematic_or_dynamic", &Body::CanBeKinematicOrDynamic,
            "Check if a body could be made kinematic or dynamic (if it was created dynamic or with mAllowDynamicOrKinematic set to true)")

        .def_prop_rw("set_is_sensor", &Body::IsSensor, &Body::SetIsSensor, "is_sensor"_a,
            "Change the body to a sensor. A sensor will receive collision callbacks, but will not cause any collision responses and can be used as a trigger volume.\n"
            "The cheapest sensor (in terms of CPU usage) is a sensor with motion type Static (they can be moved around using BodyInterface::SetPosition/SetPositionAndRotation).\n"
            "These sensors will only detect collisions with active Dynamic or Kinematic bodies. As soon as a body go to sleep, the contact point with the sensor will be lost.\n"
            "If you make a sensor Dynamic or Kinematic and activate them, the sensor will be able to detect collisions with sleeping bodies too. An active sensor will never go to sleep automatically.\n"
            "When you make a Dynamic or Kinematic sensor, make sure it is in an ObjectLayer that does not collide with Static bodies or other sensors to avoid extra overhead in the broad phase.")

        .def_prop_rw("collide_kinematic_vs_non_dynamic", &Body::GetCollideKinematicVsNonDynamic, &Body::SetCollideKinematicVsNonDynamic,
            "collide"_a,
            "If kinematic objects can generate contact points against other kinematic or static objects.\n"
            "Note that turning this on can be CPU intensive as much more collision detection work will be done without any effect on the simulation (kinematic objects are not affected by other kinematic/static objects).\n"
            "This can be used to make sensors detect static objects. Note that the sensor must be kinematic and active for it to detect static objects.")

        .def_prop_rw("use_manifold_reduction", &Body::GetUseManifoldReduction, &Body::SetUseManifoldReduction,
            "use_reduction"_a,
            "If PhysicsSettings::mUseManifoldReduction is true, this allows turning off manifold reduction for this specific body.\n"
            "Manifold reduction by default will combine contacts with similar normals that come from different SubShapeIDs (e.g. different triangles in a mesh shape or different compound shapes).\n"
            "If the application requires tracking exactly which SubShapeIDs are in contact, you can turn off manifold reduction. Note that this comes at a performance cost.\n"
            "Consider using BodyInterface::SetUseManifoldReduction if the body could already be in contact with other bodies to ensure that the contact cache is invalidated and you get the correct contact callbacks.\n")

        .def("get_use_manifold_reduction_with_body", &Body::GetUseManifoldReductionWithBody,
            "Checks if the combination of this body and inBody2 should use manifold reduction")

        .def_prop_rw("apply_gyroscopic_force", &Body::GetApplyGyroscopicForce, &Body::SetApplyGyroscopicForce,
            "apply"_a,
            "Get/Set to indicate that the gyroscopic force should be applied to this body (aka Dzhanibekov effect, see https://en.wikipedia.org/wiki/Tennis_racket_theorem)")

        .def_prop_rw("enhanced_internal_edge_removal", &Body::GetEnhancedInternalEdgeRemoval, &Body::SetEnhancedInternalEdgeRemoval,
            "apply"_a,
            "Get/Set to indicate that extra effort should be made to try to remove ghost contacts (collisions with internal edges of a mesh). This is more expensive but makes bodies move smoother over a mesh with convex edges.")

        .def("get_enhanced_internal_edge_removal_with_body", &Body::GetEnhancedInternalEdgeRemovalWithBody,
            "Checks if the combination of this body and inBody2 should use enhanced internal edge removal")

        .def_prop_rw("motion_type", &Body::GetMotionType, &Body::SetMotionType, "motion_type"_a,
            "Get/Set the motion type of this body. Consider using BodyInterface::SetMotionType instead of this function if the body may be active or if it needs to be activated.")

        .def("get_broad_phase_layer", &Body::GetBroadPhaseLayer, "Get broadphase layer, this determines in which broad phase sub-tree the object is placed")
        .def("get_object_layer", &Body::GetObjectLayer, "Get object layer, this determines which other objects it collides with")

        .def_prop_rw("collision_group", nb::overload_cast<>(&Body::GetCollisionGroup), &Body::SetCollisionGroup,
            "Get/Set Collision group and sub-group ID, determines which other objects it collides with")
        .def_prop_rw("allow_sleeping", &Body::GetAllowSleeping, &Body::SetAllowSleeping, "allow"_a,
            "If this body can go to sleep. Note that disabling sleeping on a sleeping object will not wake it up.")

        .def("reset_sleep_timer", &Body::ResetSleepTimer,
            "Resets the sleep timer. This does not wake up the body if it is sleeping, but allows resetting the system that detects when a body is sleeping.")

        .def_prop_rw("friction", &Body::GetFriction, &Body::SetFriction, "friction"_a,
            "Friction (dimensionless number, usually between 0 and 1, 0 = no friction, 1 = friction force equals force that presses the two bodies together). Note that bodies can have negative friction but the combined friction (see PhysicsSystem::SetCombineFriction) should never go below zero.")

        .def_prop_rw("restitution", &Body::GetRestitution, &Body::SetRestitution, "restitution"_a,
            "Restitution (dimensionless number, usually between 0 and 1, 0 = completely inelastic collision response, 1 = completely elastic collision response). Note that bodies can have negative restitution but the combined restitution (see PhysicsSystem::SetCombineRestitution) should never go below zero.")

        .def_prop_rw("linear_velocity", &Body::GetLinearVelocity, &Body::SetLinearVelocity, "linear_velocity"_a,
            "Get/Set world space linear velocity of the center of mass (unit: m/s).\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::SetLinearVelocity instead.")

        .def("set_linear_velocity_clamped", &Body::SetLinearVelocityClamped, "linear_velocity"_a,
            "Set world space linear velocity of the center of mass, will make sure the value is clamped against the maximum linear velocity.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::SetLinearVelocity instead.")

        .def_prop_rw("angular_velocity", &Body::GetAngularVelocity, &Body::SetAngularVelocity, "angular_velocity"_a,
            "Get/Set world space angular velocity of the center of mass (unit: rad/s).\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::SetAngularVelocity instead.")

        .def("set_angular_velocity_clamped", &Body::SetAngularVelocityClamped, "angular_velocity"_a,
            "Set world space angular velocity of the center of mass, will make sure the value is clamped against the maximum angular velocity.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::SetAngularVelocity instead.")

        .def("get_point_velocity_com", &Body::GetPointVelocityCOM, "point_relative_to_com"_a,
            "Velocity of point inPoint (in center of mass space, e.g. on the surface of the body) of the body (unit: m/s)")

        .def("get_point_velocity", &Body::GetPointVelocity, "point"_a,
            "Velocity of point inPoint (in world space, e.g. on the surface of the body) of the body (unit: m/s)")

        .def("add_force", nb::overload_cast<Vec3Arg>(&Body::AddForce), "force"_a,
            "Add force (unit: N) at center of mass for the next time step, will be reset after the next call to PhysicsSystem::Update.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::AddForce instead.")

        .def("add_force", nb::overload_cast<Vec3Arg, RVec3Arg>(&Body::AddForce), "force"_a, "position"_a,
            "Add force (unit: N) at inPosition for the next time step, will be reset after the next call to PhysicsSystem::Update.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::AddForce instead.")

        .def("add_torque", &Body::AddTorque, "torque"_a,
            "Add torque (unit: N m) for the next time step, will be reset after the next call to PhysicsSystem::Update.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::AddTorque instead.")

        .def("get_accumulated_force", &Body::GetAccumulatedForce,
            "Get the total amount of force applied to the center of mass this time step (through AddForce calls). Note that it will reset to zero after PhysicsSystem::Update.")
        .def("get_accumulated_torque", &Body::GetAccumulatedTorque,
            "Get the total amount of torque applied to the center of mass this time step (through AddForce/AddTorque calls). Note that it will reset to zero after PhysicsSystem::Update.")

        .def("reset_force", &Body::ResetForce, "Reset the total accumulated force, not that this will be done automatically after every time step.")
        .def("reset_torque", &Body::ResetTorque, "Reset the total accumulated torque, not that this will be done automatically after every time step.")
        .def("reset_motion", &Body::ResetMotion, "Reset the current velocity and accumulated force and torque.")
        .def("get_inverse_inertia", &Body::GetInverseInertia, "Get inverse inertia tensor in world space")

        .def("add_impulse", nb::overload_cast<Vec3Arg>(&Body::AddImpulse),
            "impulse"_a, "Add impulse to center of mass (unit: kg m/s).\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::AddImpulse instead.")

        .def("add_impulse", nb::overload_cast<Vec3Arg, RVec3Arg>(&Body::AddImpulse),
            "impulse"_a, "position"_a, "Add impulse to point in world space (unit: kg m/s).\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::AddImpulse instead.")

        .def("add_angular_impulse", &Body::AddAngularImpulse, "angular_impulse"_a,
            "Add angular impulse in world space (unit: N m s).\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::AddAngularImpulse instead.")

        .def("move_kinematic", &Body::MoveKinematic, "target_position"_a, "target_rotation"_a, "delta_time"_a,
            "Set velocity of body such that it will be positioned at inTargetPosition/Rotation in inDeltaTime seconds.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::MoveKinematic instead.")

        .def("apply_buoyancy_impulse", &Body::ApplyBuoyancyImpulse,
            "surface_position"_a, "surface_normal"_a, "buoyancy"_a, "linear_drag"_a,
            "angular_drag"_a, "fluid_velocity"_a, "gravity"_a, "delta_time"_a,
            "Applies an impulse to the body that simulates fluid buoyancy and drag.\n"
            "If you want the body to wake up when it is sleeping, use BodyInterface::ApplyBuoyancyImpulse instead.\n"
            "Args:\n"
            "    surface_position (Vec3): Position of the fluid surface in world space.\n"
            "    surface_normal (Vec3): Normal of the fluid surface (should point up).\n"
            "    buoyancy (float): The buoyancy factor for the body. 1 = neutral body, < 1 sinks, > 1 floats. Note that we don't use the fluid density since it is harder to configure than a simple number between [0, 2].\n"
            "    linear_drag (float): Linear drag factor that slows down the body when in the fluid (approx. 0.5).\n"
            "    angular_drag (float): Angular drag factor that slows down rotation when the body is in the fluid (approx. 0.01).\n"
            "    fluid_velocity (Vec3): The average velocity of the fluid (in m/s) in which the body resides.\n"
            "    gravity (Vec3): The gravity vector (pointing down).\n"
            "    delta_time (float): Delta time of the next simulation step (in s).\n"
            "Returns:\n"
            "    bool: true if an impulse was applied, false if the body was not in the fluid.")

        .def("is_in_broad_phase", &Body::IsInBroadPhase, "Check if this body has been added to the physics system")
        .def("is_collision_cache_invalid", &Body::IsCollisionCacheInvalid,
            "Check if this body has been changed in such a way that the collision cache should be considered invalid for any body interacting with this body")

        .def("get_shape", &Body::GetShape, nb::rv_policy::reference, "Get the shape of this body")
        .def("get_position", &Body::GetPosition, "World space position of the body")
        .def("get_rotation", &Body::GetRotation, "World space rotation of the body")
        .def("get_world_transform", &Body::GetWorldTransform, "Calculates the transform of this body")
        .def("get_center_of_mass_position", &Body::GetCenterOfMassPosition, "Gets the world space position of this body's center of mass")
        .def("get_center_of_mass_transform", &Body::GetCenterOfMassTransform, "Calculates the transform for this body's center of mass")
        .def("get_inverse_center_of_mass_transform", &Body::GetInverseCenterOfMassTransform,
            "Calculates the inverse of the transform for this body's center of mass")
        .def("get_world_space_bounds", &Body::GetWorldSpaceBounds, nb::rv_policy::reference, "Get world space bounding box")

        .def("get_motion_properties", nb::overload_cast<>(&Body::GetMotionProperties), nb::rv_policy::reference, "Access to the motion properties")

        .def("get_motion_properties_unchecked", nb::overload_cast<>(&Body::GetMotionPropertiesUnchecked), nb::rv_policy::reference,
            "Access to the motion properties (version that does not check if the object is kinematic or dynamic)")

        .def_prop_rw("user_data", &Body::GetUserData, &Body::SetUserData, "user_data"_a,
            "Access to the user data, can be used for anything by the application")

        .def("get_world_space_surface_normal", &Body::GetWorldSpaceSurfaceNormal,
            "sub_shape_id"_a, "position"_a,
            "Get surface normal of a particular sub shape and its world space surface position on this body")

        .def("get_transformed_shape", &Body::GetTransformedShape,
            "Get the transformed shape of this body, which can be used to do collision detection outside of a body lock")

        .def("get_body_creation_settings", &Body::GetBodyCreationSettings,
            "Debug function to convert a body back to a body creation settings object to be able to save/recreate the body later")
        .def("get_soft_body_creation_settings", &Body::GetSoftBodyCreationSettings,
            "Debug function to convert a soft body back to a soft body creation settings object to be able to save/recreate the body later")

        .def_ro_static("fixed_to_world", &Body::sFixedToWorld, "A dummy body that can be used by constraints to attach a constraint to the world instead of another body")

        // Internal functions
        // .def_static("find_colliding_pairs_can_collide", &Body::sFindCollidingPairsCanCollide, "@name THESE FUNCTIONS ARE FOR INTERNAL USE ONLY AND SHOULD NOT BE CALLED BY THE APPLICATION\n"
        //                                                                                       "Helper function for BroadPhase::FindCollidingPairs that returns true when two bodies can collide\n"
        //                                                                                       "It assumes that body 1 is dynamic and active and guarantees that it body 1 collides with body 2 that body 2 will not collide with body 1 in order to avoid finding duplicate collision pairs")

        // .def("add_position_step", &Body::AddPositionStep, "linear_velocity_times_delta_time"_a, "Update position using an Euler step (used during position integrate & constraint solving)")
        // .def("sub_position_step", &Body::SubPositionStep, "linear_velocity_times_delta_time"_a)

        // .def("add_rotation_step", &Body::AddRotationStep, "linear_velocity_times_delta_time"_a, "Update rotation using an Euler step (used during position integrate & constraint solving)")
        // .def("sub_rotation_step", &Body::SubRotationStep, "linear_velocity_times_delta_time"_a)

        // .def("set_in_broad_phase_internal", &Body::SetInBroadPhaseInternal, "broad_phase"_a, "Flag if body is in the broadphase (should only be called by the BroadPhase)")

        // .def("invalidate_contact_cache_internal", &Body::InvalidateContactCacheInternal, "Invalidate the contact cache (should only be called by the BodyManager), will be reset the next simulation step. Returns true if the contact cache was still valid.")
        // .def("validate_contact_cache_internal", &Body::ValidateContactCacheInternal, "Reset the collision cache invalid flag (should only be called by the BodyManager).")
        // .def("calculate_world_space_bounds_internal", &Body::CalculateWorldSpaceBoundsInternal, "Updates world space bounding box (should only be called by the PhysicsSystem)")
        // .def("set_position_and_rotation_internal", &Body::SetPositionAndRotationInternal, "position"_a, "rotation"_a, "reset_sleep_timer "_a, "Function to update body's position (should only be called by the BodyInterface since it also requires updating the broadphase)")
        // .def("update_center_of_mass_internal", &Body::UpdateCenterOfMassInternal,
        //      "previous_center_of_mass"_a,
        //      "update_mass_properties"_a,
        // "Updates the center of mass and optionally mass properties after shifting the center of mass or changes to the shape (should only be called by the BodyInterface since it also requires updating the broadphase).\n"
        // "Args:\n"
        // "    previous_center_of_mass (Vec3): Center of mass of the shape before the alterations.\n"
        // "    update_mass_properties (bool): When true, the mass and inertia tensor is recalculated.")

        // .def("set_shape_internal", &Body::SetShapeInternal, "shape"_a, "update_mass_properties"_a,
        // "Function to update a body's shape (should only be called by the BodyInterface since it also requires updating the broadphase).\n"
        // "Args:\n"
        // "    shape (Shape*): The new shape for this body.\n"
        // "    update_mass_properties (bool): When true, the mass and inertia tensor is recalculated.")

        // .def("get_index_in_active_bodies_internal", &Body::GetIndexInActiveBodiesInternal, "Access to the index in the BodyManager::mActiveBodies list")
        // .def("update_sleep_state_internal", &Body::UpdateSleepStateInternal, "delta_time"_a, "max_movement"_a, "time_before_sleep"_a, "Update eligibility for sleeping")
        // .def("save_state", &Body::SaveState, "stream"_a, "Saving state for replay")
        // .def("restore_state", &Body::RestoreState, "stream"_a, "Restoring state for replay")
        // .def_ro_static("inactive_index", &Body::cInactiveIndex)
        ;
}
