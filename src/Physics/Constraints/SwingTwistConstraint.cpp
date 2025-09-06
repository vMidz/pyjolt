#include "Common.h"
#include <Jolt/Physics/Constraints/SwingTwistConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindSwingTwistConstraint(nb::module_ &m) {
    nb::class_<SwingTwistConstraintSettings, TwoBodyConstraintSettings> swingTwistConstraintSettingsCls(m, "SwingTwistConstraintSettings",
        "Swing twist constraint settings, used to create a swing twist constraint\n"
        "All values in this structure are copied to the swing twist constraint and the settings object is no longer needed afterwards.\n"
        "This image describes the limit settings:\n"
        "@image html Docs/SwingTwistConstraint.png",
        nb::is_final());
    swingTwistConstraintSettingsCls
        .def("save_binary_state", &SwingTwistConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &SwingTwistConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &SwingTwistConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("position1", &SwingTwistConstraintSettings::mPosition1,
            "///@name Body 1 constraint reference frame (space determined by mSpace)")
        .def_rw("twist_axis1", &SwingTwistConstraintSettings::mTwistAxis1)
        .def_rw("plane_axis1", &SwingTwistConstraintSettings::mPlaneAxis1)
        .def_rw("position2", &SwingTwistConstraintSettings::mPosition2,
            "///@name Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("twist_axis2", &SwingTwistConstraintSettings::mTwistAxis2)
        .def_rw("plane_axis2", &SwingTwistConstraintSettings::mPlaneAxis2)
        .def_rw("swing_type", &SwingTwistConstraintSettings::mSwingType,
            "The type of swing constraint that we want to use.")
        .def_rw("normal_half_cone_angle", &SwingTwistConstraintSettings::mNormalHalfConeAngle,
            "///@name Swing rotation limits")
        .def_rw("plane_half_cone_angle", &SwingTwistConstraintSettings::mPlaneHalfConeAngle,
            "See image at Detailed Description. Angle in radians.")
        .def_rw("twist_min_angle", &SwingTwistConstraintSettings::mTwistMinAngle,
            "///@name Twist rotation limits")
        .def_rw("twist_max_angle", &SwingTwistConstraintSettings::mTwistMaxAngle,
            "See image at Detailed Description. Angle in radians. Should be \\f$\\in [-\\pi, \\pi]\\f$.")
        .def_rw("max_friction_torque", &SwingTwistConstraintSettings::mMaxFrictionTorque,
            "///@name Friction")
        .def_rw("swing_motor_settings", &SwingTwistConstraintSettings::mSwingMotorSettings,
            "///@name In case the constraint is powered, this determines the motor settings around the swing and twist axis")
        .def_rw("twist_motor_settings", &SwingTwistConstraintSettings::mTwistMotorSettings);

    nb::class_<SwingTwistConstraint, TwoBodyConstraint> swingTwistConstraintCls(m, "SwingTwistConstraint",
        "A swing twist constraint is a specialized constraint for humanoid ragdolls that allows limited rotation only\n"
        "@see SwingTwistConstraintSettings for a description of the limits",
        nb::is_final());
    swingTwistConstraintCls
        .def(nb::init<Body &, Body &, const SwingTwistConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct swing twist constraint")
        .def("get_sub_type", &SwingTwistConstraint::GetSubType,
            "///@name Generic interface of a constraint")
        .def("notify_shape_changed", &SwingTwistConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &SwingTwistConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &SwingTwistConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &SwingTwistConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &SwingTwistConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &SwingTwistConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &SwingTwistConstraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &SwingTwistConstraint::DrawConstraintLimits, "renderer"_a)
        .def("save_state", &SwingTwistConstraint::SaveState, "stream"_a)
        .def("restore_state", &SwingTwistConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &SwingTwistConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &SwingTwistConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &SwingTwistConstraint::GetConstraintToBody2Matrix)
        .def("get_local_space_position1", &SwingTwistConstraint::GetLocalSpacePosition1,
            "///@name Constraint reference frame")
        .def("get_local_space_position2", &SwingTwistConstraint::GetLocalSpacePosition2)
        .def("get_constraint_to_body1", &SwingTwistConstraint::GetConstraintToBody1)
        .def("get_constraint_to_body2", &SwingTwistConstraint::GetConstraintToBody2)
        .def("get_normal_half_cone_angle", &SwingTwistConstraint::GetNormalHalfConeAngle,
            "///@name Constraint limits")
        .def("set_normal_half_cone_angle", &SwingTwistConstraint::SetNormalHalfConeAngle, "angle"_a)
        .def("get_plane_half_cone_angle", &SwingTwistConstraint::GetPlaneHalfConeAngle)
        .def("set_plane_half_cone_angle", &SwingTwistConstraint::SetPlaneHalfConeAngle, "angle"_a)
        .def("get_twist_min_angle", &SwingTwistConstraint::GetTwistMinAngle)
        .def("set_twist_min_angle", &SwingTwistConstraint::SetTwistMinAngle, "angle"_a)
        .def("get_twist_max_angle", &SwingTwistConstraint::GetTwistMaxAngle)
        .def("set_twist_max_angle", &SwingTwistConstraint::SetTwistMaxAngle, "angle"_a)
        .def("get_swing_motor_settings", nb::overload_cast<>(&SwingTwistConstraint::GetSwingMotorSettings),
            "///@name Motor settings")
        .def("get_twist_motor_settings", nb::overload_cast<>(&SwingTwistConstraint::GetTwistMotorSettings))
        .def("set_max_friction_torque", &SwingTwistConstraint::SetMaxFrictionTorque, "friction_torque"_a,
            "///@name Friction control")
        .def("get_max_friction_torque", &SwingTwistConstraint::GetMaxFrictionTorque)
        .def("set_swing_motor_state", &SwingTwistConstraint::SetSwingMotorState, "state"_a,
            "Controls if the motors are on or off")
        .def("get_swing_motor_state", &SwingTwistConstraint::GetSwingMotorState)
        .def("set_twist_motor_state", &SwingTwistConstraint::SetTwistMotorState, "state"_a)
        .def("get_twist_motor_state", &SwingTwistConstraint::GetTwistMotorState)
        .def("set_target_angular_velocity_cs", &SwingTwistConstraint::SetTargetAngularVelocityCS, "angular_velocity"_a,
            "Set the target angular velocity of body 2 in constraint space of body 2")
        .def("get_target_angular_velocity_cs", &SwingTwistConstraint::GetTargetAngularVelocityCS)
        .def("set_target_orientation_cs", &SwingTwistConstraint::SetTargetOrientationCS, "orientation"_a,
            "Set the target orientation in constraint space (drives constraint to: GetRotationInConstraintSpace() == inOrientation)")
        .def("get_target_orientation_cs", &SwingTwistConstraint::GetTargetOrientationCS)
        .def("set_target_orientation_bs", &SwingTwistConstraint::SetTargetOrientationBS, "orientation"_a,
            "Set the target orientation in body space (R2 = R1 * inOrientation, where R1 and R2 are the world space rotations for body 1 and 2).\n"
            "Solve: R2 * ConstraintToBody2 = R1 * ConstraintToBody1 * q (see SwingTwistConstraint::GetSwingTwist) and R2 = R1 * inOrientation for q.")
        .def("get_rotation_in_constraint_space", &SwingTwistConstraint::GetRotationInConstraintSpace,
            "Get current rotation of constraint in constraint space.\n"
            "Solve: R2 * ConstraintToBody2 = R1 * ConstraintToBody1 * q for q.")
        .def("get_total_lambda_position", &SwingTwistConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_twist", &SwingTwistConstraint::GetTotalLambdaTwist)
        .def("get_total_lambda_swing_y", &SwingTwistConstraint::GetTotalLambdaSwingY)
        .def("get_total_lambda_swing_z", &SwingTwistConstraint::GetTotalLambdaSwingZ)
        .def("get_total_lambda_motor", &SwingTwistConstraint::GetTotalLambdaMotor);
}