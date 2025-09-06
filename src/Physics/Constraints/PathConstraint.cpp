#include "Common.h"
#include <Jolt/Physics/Constraints/PathConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindPathConstraint(nb::module_ &m) {
    nb::enum_<EPathRotationConstraintType>(m, "EPathRotationConstraintType",
        "How to constrain the rotation of the body to a PathConstraint")
        .value("FREE", EPathRotationConstraintType::Free,
            "Do not constrain the rotation of the body at all")
        .value("CONSTRAIN_AROUND_TANGENT", EPathRotationConstraintType::ConstrainAroundTangent,
            "Only allow rotation around the tangent vector (following the path)")
        .value("CONSTRAIN_AROUND_NORMAL", EPathRotationConstraintType::ConstrainAroundNormal,
            "Only allow rotation around the normal vector (perpendicular to the path)")
        .value("CONSTRAIN_AROUND_BINORMAL", EPathRotationConstraintType::ConstrainAroundBinormal,
            "Only allow rotation around the binormal vector (perpendicular to the path)")
        .value("CONSTRAIN_TO_PATH", EPathRotationConstraintType::ConstrainToPath,
            "Fully constrain the rotation of body 2 to the path (following the tangent and normal of the path)")
        .value("FULLY_CONSTRAINED", EPathRotationConstraintType::FullyConstrained,
            "Fully constrain the rotation of the body 2 to the rotation of body 1");
    nb::class_<PathConstraintSettings, TwoBodyConstraintSettings> pathConstraintSettingsCls(m, "PathConstraintSettings",
        "Path constraint settings, used to constrain the degrees of freedom between two bodies to a path\n"
        "The requirements of the path are that:\n"
        "* Tangent, normal and bi-normal form an orthonormal basis with: tangent cross bi-normal = normal\n"
        "* The path points along the tangent vector\n"
        "* The path is continuous so doesn't contain any sharp corners\n"
        "The reason for all this is that the constraint acts like a slider constraint with the sliding axis being the tangent vector (the assumption here is that delta time will be small enough so that the path is linear for that delta time).",
        nb::is_final());
    pathConstraintSettingsCls
        .def("save_binary_state", &PathConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &PathConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("path", &PathConstraintSettings::mPath,
            "The path that constrains the two bodies")
        .def_rw("path_position", &PathConstraintSettings::mPathPosition,
            "The position of the path start relative to world transform of body 1")
        .def_rw("path_rotation", &PathConstraintSettings::mPathRotation,
            "The rotation of the path start relative to world transform of body 1")
        .def_rw("path_fraction", &PathConstraintSettings::mPathFraction,
            "The fraction along the path that corresponds to the initial position of body 2. Usually this is 0, the beginning of the path. But if you want to start an object halfway the path you can calculate this with mPath->GetClosestPoint(point on path to attach body to).")
        .def_rw("max_friction_force", &PathConstraintSettings::mMaxFrictionForce,
            "Maximum amount of friction force to apply (N) when not driven by a motor.")
        .def_rw("position_motor_settings", &PathConstraintSettings::mPositionMotorSettings,
            "In case the constraint is powered, this determines the motor settings along the path")
        .def_rw("rotation_constraint_type", &PathConstraintSettings::mRotationConstraintType,
            "How to constrain the rotation of the body to the path");

    nb::class_<PathConstraint, TwoBodyConstraint> pathConstraintCls(m, "PathConstraint",
        "Path constraint, used to constrain the degrees of freedom between two bodies to a path",
        nb::is_final());
    pathConstraintCls
        .def(nb::init<Body &, Body &, const PathConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct point constraint")
        .def("get_sub_type", &PathConstraint::GetSubType)
        .def("notify_shape_changed", &PathConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &PathConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &PathConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &PathConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &PathConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &PathConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &PathConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &PathConstraint::SaveState, "stream"_a)
        .def("restore_state", &PathConstraint::RestoreState, "stream"_a)
        .def("is_active", &PathConstraint::IsActive)
        .def("get_constraint_settings", &PathConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &PathConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &PathConstraint::GetConstraintToBody2Matrix)
        .def("set_path", &PathConstraint::SetPath, "path"_a, "path_fraction"_a,
            "Update the path for this constraint")
        .def("get_path", &PathConstraint::GetPath, nb::rv_policy::reference,
            "Access to the current path")
        .def("get_path_fraction", &PathConstraint::GetPathFraction,
            "Access to the current fraction along the path e [0, GetPath()->GetMaxPathFraction()]")
        .def("set_max_friction_force", &PathConstraint::SetMaxFrictionForce, "friction_force"_a,
            "Friction control")
        .def("get_max_friction_force", &PathConstraint::GetMaxFrictionForce)
        .def("get_position_motor_settings", nb::overload_cast<>(&PathConstraint::GetPositionMotorSettings),
            "Position motor settings")
        .def("set_position_motor_state", &PathConstraint::SetPositionMotorState, "state"_a)
        .def("get_position_motor_state", &PathConstraint::GetPositionMotorState)
        .def("set_target_velocity", &PathConstraint::SetTargetVelocity, "velocity"_a)
        .def("get_target_velocity", &PathConstraint::GetTargetVelocity)
        .def("set_target_path_fraction", &PathConstraint::SetTargetPathFraction, "fraction"_a)
        .def("get_target_path_fraction", &PathConstraint::GetTargetPathFraction)
        .def("get_total_lambda_position", &PathConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_position_limits", &PathConstraint::GetTotalLambdaPositionLimits)
        .def("get_total_lambda_motor", &PathConstraint::GetTotalLambdaMotor)
        .def("get_total_lambda_rotation_hinge", &PathConstraint::GetTotalLambdaRotationHinge)
        .def("get_total_lambda_rotation", &PathConstraint::GetTotalLambdaRotation);
}