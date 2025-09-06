#include "Common.h"
#include <Jolt/Physics/Body/MotionProperties.h>
#include <Jolt/Physics/StateRecorder.h>
void BindMotionProperties(nb::module_ &m) {
    nb::enum_<ECanSleep>(m, "ECanSleep",
        "Enum that determines if an object can go to sleep")
        .value("CANNOT_SLEEP", ECanSleep::CannotSleep, "Object cannot go to sleep")
        .value("CAN_SLEEP", ECanSleep::CanSleep, "Object can go to sleep");

    nb::class_<MotionProperties>(m, "MotionProperties",
        "The Body class only keeps track of state for static bodies, the MotionProperties class keeps the additional state needed for a moving Body. It has a 1-on-1 relationship with the body.")
        .def("get_motion_quality", &MotionProperties::GetMotionQuality,
            "Motion quality, or how well it detects collisions when it has a high velocity")
        .def("get_allowed_do_fs", &MotionProperties::GetAllowedDOFs,
            "Get the allowed degrees of freedom that this body has (this can be changed by calling SetMassProperties)")
        .def("get_allow_sleeping", &MotionProperties::GetAllowSleeping,
            "If this body can go to sleep.")
        .def("get_linear_velocity", &MotionProperties::GetLinearVelocity,
            "Get world space linear velocity of the center of mass")
        .def("set_linear_velocity", &MotionProperties::SetLinearVelocity,
            "linear_velocity"_a, "Set world space linear velocity of the center of mass")
        .def("set_linear_velocity_clamped", &MotionProperties::SetLinearVelocityClamped,
            "linear_velocity"_a,
            "Set world space linear velocity of the center of mass, will make sure the value is clamped against the maximum linear velocity")
        .def("get_angular_velocity", &MotionProperties::GetAngularVelocity,
            "Get world space angular velocity of the center of mass")
        .def("set_angular_velocity", &MotionProperties::SetAngularVelocity,
            "angular_velocity"_a, "Set world space angular velocity of the center of mass")
        .def("set_angular_velocity_clamped", &MotionProperties::SetAngularVelocityClamped, "angular_velocity"_a,
            "Set world space angular velocity of the center of mass, will make sure the value is clamped against the maximum angular velocity")
        .def("move_kinematic", &MotionProperties::MoveKinematic,
            "delta_position"_a, "delta_rotation"_a, "delta_time"_a,
            "Set velocity of body such that it will be rotate/translate by inDeltaPosition/Rotation in inDeltaTime seconds.")
        .def("get_max_linear_velocity", &MotionProperties::GetMaxLinearVelocity,
            "Maximum linear velocity that a body can achieve. Used to prevent the system from exploding.")
        .def("set_max_linear_velocity", &MotionProperties::SetMaxLinearVelocity, "linear_velocity"_a)
        .def("get_max_angular_velocity", &MotionProperties::GetMaxAngularVelocity,
            "Maximum angular velocity that a body can achieve. Used to prevent the system from exploding.")
        .def("set_max_angular_velocity", &MotionProperties::SetMaxAngularVelocity, "angular_velocity"_a)
        .def("clamp_linear_velocity", &MotionProperties::ClampLinearVelocity, "Clamp velocity according to limit")
        .def("clamp_angular_velocity", &MotionProperties::ClampAngularVelocity)
        .def("get_linear_damping", &MotionProperties::GetLinearDamping,
            "Get linear damping: dv/dt = -c * v. c must be between 0 and 1 but is usually close to 0.")
        .def("set_linear_damping", &MotionProperties::SetLinearDamping, "linear_damping"_a)
        .def("get_angular_damping", &MotionProperties::GetAngularDamping,
            "Get angular damping: dw/dt = -c * w. c must be between 0 and 1 but is usually close to 0.")
        .def("set_angular_damping", &MotionProperties::SetAngularDamping, "angular_damping"_a)
        .def("get_gravity_factor", &MotionProperties::GetGravityFactor, "Get gravity factor (1 = normal gravity, 0 = no gravity)")
        .def("set_gravity_factor", &MotionProperties::SetGravityFactor, "gravity_factor"_a)
        .def("set_mass_properties", &MotionProperties::SetMassProperties,
            "allowed_do_fs"_a, "mass_properties"_a, "Set the mass and inertia tensor")
        .def("get_inverse_mass", &MotionProperties::GetInverseMass,
            "Get inverse mass (1 / mass). Should only be called on a dynamic object (static or kinematic bodies have infinite mass so should be treated as 1 / mass = 0)")
        .def("get_inverse_mass_unchecked", &MotionProperties::GetInverseMassUnchecked)
        .def("set_inverse_mass", &MotionProperties::SetInverseMass,
            "inverse_mass"_a, "Set the inverse mass (1 / mass).\n"
            "Note that mass and inertia are linearly related (e.g. inertia of a sphere with mass m and radius r is \\f$2/5 \\: m \\: r^2\\f$).\n"
            "If you change mass, inertia should probably change as well. See MassProperties::ScaleToMass.\n"
            "If all your translation degrees of freedom are restricted, make sure this is zero (see EAllowedDOFs).")
        .def("get_inverse_inertia_diagonal", &MotionProperties::GetInverseInertiaDiagonal,
            "Diagonal of inverse inertia matrix: D. Should only be called on a dynamic object (static or kinematic bodies have infinite mass so should be treated as D = 0)")
        .def("get_inertia_rotation", &MotionProperties::GetInertiaRotation,
            "Rotation (R) that takes inverse inertia diagonal to local space: \\f$I_{body}^{-1} = R \\: D \\: R^{-1}\\f$")
        .def("set_inverse_inertia", &MotionProperties::SetInverseInertia,
            "diagonal"_a, "rot"_a,
            "Set the inverse inertia tensor in local space by setting the diagonal and the rotation: \\f$I_{body}^{-1} = R \\: D \\: R^{-1}\\f$.\n"
            "Note that mass and inertia are linearly related (e.g. inertia of a sphere with mass m and radius r is \\f$2/5 \\: m \\: r^2\\f$).\n"
            "If you change inertia, mass should probably change as well. See MassProperties::ScaleToMass.\n"
            "If all your rotation degrees of freedom are restricted, make sure this is zero (see EAllowedDOFs).")
        .def("get_local_space_inverse_inertia", &MotionProperties::GetLocalSpaceInverseInertia,
            "Get inverse inertia matrix (\\f$I_{body}^{-1}\\f$). Will be a matrix of zeros for a static or kinematic object.")
        .def("get_local_space_inverse_inertia_unchecked", &MotionProperties::GetLocalSpaceInverseInertiaUnchecked,
            "Same as GetLocalSpaceInverseInertia() but doesn't check if the body is dynamic")
        .def("get_inverse_inertia_for_rotation", &MotionProperties::GetInverseInertiaForRotation, "rotation"_a,
            "Get inverse inertia matrix (\\f$I^{-1}\\f$) for a given object rotation (translation will be ignored). Zero if object is static or kinematic.")
        .def("multiply_world_space_inverse_inertia_by_vector", &MotionProperties::MultiplyWorldSpaceInverseInertiaByVector,
            "body_rotation"_a, "v"_a,
            "Multiply a vector with the inverse world space inertia tensor (\\f$I_{world}^{-1}\\f$). Zero if object is static or kinematic.")
        .def("get_point_velocity_com", &MotionProperties::GetPointVelocityCOM, "point_relative_to_com"_a,
            "Velocity of point inPoint (in center of mass space, e.g. on the surface of the body) of the body (unit: m/s)")
        .def("get_accumulated_force", &MotionProperties::GetAccumulatedForce)
        .def("get_accumulated_torque", &MotionProperties::GetAccumulatedTorque)
        .def("reset_force", &MotionProperties::ResetForce)
        .def("reset_torque", &MotionProperties::ResetTorque)
        .def("reset_motion", &MotionProperties::ResetMotion)
        .def("get_linear_do_fs_mask", &MotionProperties::GetLinearDOFsMask,
            "Returns a vector where the linear components that are not allowed by mAllowedDOFs are set to 0 and the rest to 0xffffffff")
        .def("lock_translation", &MotionProperties::LockTranslation,
            "v"_a, "Takes a translation vector inV and returns a vector where the components that are not allowed by mAllowedDOFs are set to 0")
        .def("get_angular_do_fs_mask", &MotionProperties::GetAngularDOFsMask,
            "Returns a vector where the angular components that are not allowed by mAllowedDOFs are set to 0 and the rest to 0xffffffff")
        .def("lock_angular", &MotionProperties::LockAngular,
            "v"_a, "Takes an angular velocity / torque vector inV and returns a vector where the components that are not allowed by mAllowedDOFs are set to 0")
        .def("set_num_velocity_steps_override", &MotionProperties::SetNumVelocityStepsOverride, "n"_a,
            "Used only when this body is dynamic and colliding. Override for the number of solver velocity iterations to run, 0 means use the default in PhysicsSettings::mNumVelocitySteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def("get_num_velocity_steps_override", &MotionProperties::GetNumVelocityStepsOverride)
        .def("set_num_position_steps_override", &MotionProperties::SetNumPositionStepsOverride, "n"_a,
            "Used only when this body is dynamic and colliding. Override for the number of solver position iterations to run, 0 means use the default in PhysicsSettings::mNumPositionSteps. The number of iterations to use is the max of all contacts and constraints in the island.")
        .def("get_num_position_steps_override", &MotionProperties::GetNumPositionStepsOverride)
        .def("add_linear_velocity_step", &MotionProperties::AddLinearVelocityStep,
            "linear_velocity_change"_a, "@name Update linear and angular velocity (used during constraint solving)\n")
        .def("sub_linear_velocity_step", &MotionProperties::SubLinearVelocityStep, "linear_velocity_change"_a)
        .def("add_angular_velocity_step", &MotionProperties::AddAngularVelocityStep, "angular_velocity_change"_a)
        .def("sub_angular_velocity_step", &MotionProperties::SubAngularVelocityStep, "angular_velocity_change"_a)
        .def("apply_gyroscopic_force_internal", &MotionProperties::ApplyGyroscopicForceInternal,
            "body_rotation"_a, "delta_time"_a,
            "Apply the gyroscopic force (aka Dzhanibekov effect, see https://en.wikipedia.org/wiki/Tennis_racket_theorem)")
        .def("apply_force_torque_and_drag_internal", &MotionProperties::ApplyForceTorqueAndDragInternal,
            "body_rotation"_a, "gravity"_a, "delta_time"_a, "Apply all accumulated forces, torques and drag (should only be called by the PhysicsSystem)")
        .def("get_island_index_internal", &MotionProperties::GetIslandIndexInternal, "Access to the island index")
        .def("set_island_index_internal", &MotionProperties::SetIslandIndexInternal, "index"_a)
        .def("get_index_in_active_bodies_internal", &MotionProperties::GetIndexInActiveBodiesInternal,
            "Access to the index in the active bodies array")
        .def("reset_sleep_test_spheres", &MotionProperties::ResetSleepTestSpheres,
            "points"_a, "Reset spheres to center around inPoints with radius 0")
        .def("reset_sleep_test_timer", &MotionProperties::ResetSleepTestTimer,
            "Reset the sleep test timer without resetting the sleep test spheres")
        .def("accumulate_sleep_time", &MotionProperties::AccumulateSleepTime,
            "delta_time"_a, "time_before_sleep"_a, "Accumulate sleep time and return if a body can go to sleep")
        .def("save_state", &MotionProperties::SaveState, "stream"_a, "Saving state for replay")
        .def("restore_state", &MotionProperties::RestoreState, "stream"_a, "Restoring state for replay")
        .def_ro_static("C_INACTIVE_INDEX", &MotionProperties::cInactiveIndex, "Constant indicating that body is not active");
}