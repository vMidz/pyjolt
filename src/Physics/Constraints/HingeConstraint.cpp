#include "Common.h"
#include <Jolt/Physics/Constraints/HingeConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindHingeConstraint(nb::module_ &m) {
    nb::class_<HingeConstraintSettings, TwoBodyConstraintSettings> hingeConstraintSettingsCls(m, "HingeConstraintSettings",
        "Hinge constraint settings, used to create a hinge constraint",
        nb::is_final());
    hingeConstraintSettingsCls
        .def("save_binary_state", &HingeConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &HingeConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &HingeConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("point1", &HingeConstraintSettings::mPoint1,
            "Body 1 constraint reference frame (space determined by mSpace).\n"
            "Hinge axis is the axis where rotation is allowed.\n"
            "When the normal axis of both bodies align in world space, the hinge angle is defined to be 0.\n"
            "mHingeAxis1 and mNormalAxis1 should be perpendicular. mHingeAxis2 and mNormalAxis2 should also be perpendicular.\n"
            "If you configure the joint in world space and create both bodies with a relative rotation you want to be defined as zero,\n"
            "you can simply set mHingeAxis1 = mHingeAxis2 and mNormalAxis1 = mNormalAxis2.")
        .def_rw("hinge_axis1", &HingeConstraintSettings::mHingeAxis1)
        .def_rw("normal_axis1", &HingeConstraintSettings::mNormalAxis1)
        .def_rw("point2", &HingeConstraintSettings::mPoint2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("hinge_axis2", &HingeConstraintSettings::mHingeAxis2)
        .def_rw("normal_axis2", &HingeConstraintSettings::mNormalAxis2)
        .def_rw("limits_min", &HingeConstraintSettings::mLimitsMin,
            "Rotation around the hinge axis will be limited between [mLimitsMin, mLimitsMax] where mLimitsMin e [-pi, 0] and mLimitsMax e [0, pi].\n"
            "Both angles are in radians.")
        .def_rw("limits_max", &HingeConstraintSettings::mLimitsMax)
        .def_rw("limits_spring_settings", &HingeConstraintSettings::mLimitsSpringSettings,
            "When enabled, this makes the limits soft. When the constraint exceeds the limits, a spring force will pull it back.")
        .def_rw("max_friction_torque", &HingeConstraintSettings::mMaxFrictionTorque,
            "Maximum amount of torque (N m) to apply as friction when the constraint is not powered by a motor")
        .def_rw("motor_settings", &HingeConstraintSettings::mMotorSettings,
            "In case the constraint is powered, this determines the motor settings around the hinge axis");

    nb::class_<HingeConstraint, TwoBodyConstraint> hingeConstraintCls(m, "HingeConstraint",
        "A hinge constraint constrains 2 bodies on a single point and allows only a single axis of rotation",
        nb::is_final());
    hingeConstraintCls
        .def(nb::init<Body &, Body &, const HingeConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct hinge constraint")
        .def("get_sub_type", &HingeConstraint::GetSubType)
        .def("notify_shape_changed", &HingeConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &HingeConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &HingeConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &HingeConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &HingeConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &HingeConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &HingeConstraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &HingeConstraint::DrawConstraintLimits, "renderer"_a)
        .def("save_state", &HingeConstraint::SaveState, "stream"_a)
        .def("restore_state", &HingeConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &HingeConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &HingeConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &HingeConstraint::GetConstraintToBody2Matrix)
        .def("get_local_space_point1", &HingeConstraint::GetLocalSpacePoint1,
            "Get the attachment point for body 1 relative to body 1 COM (transform by Body::GetCenterOfMassTransform to take to world space)")
        .def("get_local_space_point2", &HingeConstraint::GetLocalSpacePoint2,
            "Get the attachment point for body 2 relative to body 2 COM (transform by Body::GetCenterOfMassTransform to take to world space)")
        .def("get_local_space_hinge_axis1", &HingeConstraint::GetLocalSpaceHingeAxis1)
        .def("get_local_space_hinge_axis2", &HingeConstraint::GetLocalSpaceHingeAxis2)
        .def("get_local_space_normal_axis1", &HingeConstraint::GetLocalSpaceNormalAxis1)
        .def("get_local_space_normal_axis2", &HingeConstraint::GetLocalSpaceNormalAxis2)
        .def("get_current_angle", &HingeConstraint::GetCurrentAngle,
            "Get the current rotation angle from the rest position")
        .def("set_max_friction_torque", &HingeConstraint::SetMaxFrictionTorque, "friction_torque"_a)
        .def("get_max_friction_torque", &HingeConstraint::GetMaxFrictionTorque)
        .def("get_motor_settings", nb::overload_cast<>(&HingeConstraint::GetMotorSettings))
        .def("set_motor_state", &HingeConstraint::SetMotorState, "state"_a)
        .def("get_motor_state", &HingeConstraint::GetMotorState)
        .def("set_target_angular_velocity", &HingeConstraint::SetTargetAngularVelocity, "angular_velocity"_a)
        .def("get_target_angular_velocity", &HingeConstraint::GetTargetAngularVelocity,
            "rad/s")
        .def("set_target_angle", &HingeConstraint::SetTargetAngle, "angle"_a)
        .def("get_target_angle", &HingeConstraint::GetTargetAngle,
            "rad")
        .def("set_limits", &HingeConstraint::SetLimits, "limits_min"_a, "limits_max"_a,
            "Update the rotation limits of the hinge, value in radians (see HingeConstraintSettings)")
        .def("get_limits_min", &HingeConstraint::GetLimitsMin)
        .def("get_limits_max", &HingeConstraint::GetLimitsMax)
        .def("has_limits", &HingeConstraint::HasLimits)
        .def("get_limits_spring_settings", nb::overload_cast<>(&HingeConstraint::GetLimitsSpringSettings),
            "Update the limits spring settings")
        .def("set_limits_spring_settings", &HingeConstraint::SetLimitsSpringSettings, "limits_spring_settings"_a)
        .def("get_total_lambda_position", &HingeConstraint::GetTotalLambdaPosition,
            "@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_rotation", &HingeConstraint::GetTotalLambdaRotation)
        .def("get_total_lambda_rotation_limits", &HingeConstraint::GetTotalLambdaRotationLimits)
        .def("get_total_lambda_motor", &HingeConstraint::GetTotalLambdaMotor);
}