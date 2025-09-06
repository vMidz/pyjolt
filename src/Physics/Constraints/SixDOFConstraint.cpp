#include "Common.h"
#include <Jolt/Physics/Constraints/SixDOFConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindSixDOFConstraint(nb::module_ &m) {
    // TODO: arrays
    nb::class_<SixDOFConstraintSettings, TwoBodyConstraintSettings> sixDOFConstraintSettingsCls(m, "SixDOFConstraintSettings",
        "6 Degree Of Freedom Constraint setup structure. Allows control over each of the 6 degrees of freedom.",
        nb::is_final());
    sixDOFConstraintSettingsCls
        .def("save_binary_state", &SixDOFConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &SixDOFConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def("make_free_axis", &SixDOFConstraintSettings::MakeFreeAxis, "axis"_a,
            "Make axis free (unconstrained)")
        .def("is_free_axis", &SixDOFConstraintSettings::IsFreeAxis, "axis"_a)
        .def("make_fixed_axis", &SixDOFConstraintSettings::MakeFixedAxis, "axis"_a,
            "Make axis fixed (fixed at value 0)")
        .def("is_fixed_axis", &SixDOFConstraintSettings::IsFixedAxis, "axis"_a)
        .def("set_limited_axis", &SixDOFConstraintSettings::SetLimitedAxis, "axis"_a, "min"_a, "max"_a,
            "Set a valid range for the constraint (if inMax < inMin, the axis will become fixed)")
        .def_rw("space", &SixDOFConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("position1", &SixDOFConstraintSettings::mPosition1,
            "Body 1 constraint reference frame (space determined by mSpace)")
        .def_rw("axis_x1", &SixDOFConstraintSettings::mAxisX1)
        .def_rw("axis_y1", &SixDOFConstraintSettings::mAxisY1)
        .def_rw("position2", &SixDOFConstraintSettings::mPosition2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("axis_x2", &SixDOFConstraintSettings::mAxisX2)
        .def_rw("axis_y2", &SixDOFConstraintSettings::mAxisY2)
        // .def_rw("max_friction", &SixDOFConstraintSettings::mMaxFriction,
            // "Friction settings.\n"
            // "For translation: Max friction force in N. 0 = no friction.\n"
            // "For rotation: Max friction torque in Nm. 0 = no friction.")
        // .def_rw("swing_type", &SixDOFConstraintSettings::mSwingType,
            // "The type of swing constraint that we want to use.")
        // .def_rw("limit_min", &SixDOFConstraintSettings::mLimitMin,
            // "Limits.\n"
            // "For translation: Min and max linear limits in m (0 is frame of body 1 and 2 coincide).\n"
            // "For rotation: Min and max angular limits in rad (0 is frame of body 1 and 2 coincide). See comments at Axis enum for limit ranges.\n"
            // "Remove degree of freedom by setting min = FLT_MAX and max = -FLT_MAX. The constraint will be driven to 0 for this axis.\n"
            // "Free movement over an axis is allowed when min = -FLT_MAX and max = FLT_MAX.\n"
            // "Rotation limit around X-Axis: When limited, should be \\f$\\in [-\\pi, \\pi]\\f$. Can be asymmetric around zero.\n"
            // "Rotation limit around Y-Z Axis: Forms a pyramid or cone shaped limit:\n"
            // "* For pyramid, should be \\f$\\in [-\\pi, \\pi]\\f$ and does not need to be symmetrical around zero.\n"
            // "* For cone should be \\f$\\in [0, \\pi]\\f$ and needs to be symmetrical around zero (min limit is assumed to be -max limit).")
        // .def_rw("limit_max", &SixDOFConstraintSettings::mLimitMax)
        // .def_rw("limits_spring_settings", &SixDOFConstraintSettings::mLimitsSpringSettings,
            // "When enabled, this makes the limits soft. When the constraint exceeds the limits, a spring force will pull it back.\n"
            // "Only soft translation limits are supported, soft rotation limits are not currently supported.")
        // .def_rw("motor_settings", &SixDOFConstraintSettings::mMotorSettings,
            // "Motor settings for each axis")
            ;

    nb::enum_<SixDOFConstraintSettings::EAxis>(m, "EAxis",
        "Constraint is split up into translation/rotation around X, Y and Z axis.")
        .value("TRANSLATION_X", SixDOFConstraintSettings::EAxis::TranslationX)
        .value("TRANSLATION_Y", SixDOFConstraintSettings::EAxis::TranslationY)
        .value("TRANSLATION_Z", SixDOFConstraintSettings::EAxis::TranslationZ)
        .value("ROTATION_X", SixDOFConstraintSettings::EAxis::RotationX)
        .value("ROTATION_Y", SixDOFConstraintSettings::EAxis::RotationY)
        .value("ROTATION_Z", SixDOFConstraintSettings::EAxis::RotationZ)
        .value("NUM", SixDOFConstraintSettings::EAxis::Num)
        .value("NUM_TRANSLATION", SixDOFConstraintSettings::EAxis::NumTranslation);

    nb::class_<SixDOFConstraint, TwoBodyConstraint> sixDOFConstraintCls(m, "SixDOFConstraint",
        "6 Degree Of Freedom Constraint. Allows control over each of the 6 degrees of freedom.",
        nb::is_final());
    sixDOFConstraintCls
        .def(nb::init<Body &, Body &, const SixDOFConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct six DOF constraint")
        .def("get_sub_type", &SixDOFConstraint::GetSubType,
            "Generic interface of a constraint")
        .def("notify_shape_changed", &SixDOFConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &SixDOFConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &SixDOFConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &SixDOFConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &SixDOFConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &SixDOFConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &SixDOFConstraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &SixDOFConstraint::DrawConstraintLimits, "renderer"_a)
        .def("save_state", &SixDOFConstraint::SaveState, "stream"_a)
        .def("restore_state", &SixDOFConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &SixDOFConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &SixDOFConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &SixDOFConstraint::GetConstraintToBody2Matrix)
        .def("set_translation_limits", &SixDOFConstraint::SetTranslationLimits, "limit_min"_a, "limit_max"_a,
            "Update the translation limits for this constraint")
        .def("set_rotation_limits", &SixDOFConstraint::SetRotationLimits, "limit_min"_a, "limit_max"_a,
            "Update the rotational limits for this constraint")
        .def("get_limits_min", &SixDOFConstraint::GetLimitsMin, "axis"_a,
            "Get constraint Limits")
        .def("get_limits_max", &SixDOFConstraint::GetLimitsMax, "axis"_a)
        .def("get_translation_limits_min", &SixDOFConstraint::GetTranslationLimitsMin)
        .def("get_translation_limits_max", &SixDOFConstraint::GetTranslationLimitsMax)
        .def("get_rotation_limits_min", &SixDOFConstraint::GetRotationLimitsMin)
        .def("get_rotation_limits_max", &SixDOFConstraint::GetRotationLimitsMax)
        .def("is_fixed_axis", &SixDOFConstraint::IsFixedAxis, "axis"_a,
            "Check which axis are fixed/free")
        .def("is_free_axis", &SixDOFConstraint::IsFreeAxis, "axis"_a)
        .def("get_limits_spring_settings", &SixDOFConstraint::GetLimitsSpringSettings, "axis"_a,
            "Update the limits spring settings")
        .def("set_limits_spring_settings", &SixDOFConstraint::SetLimitsSpringSettings, "axis"_a, "limits_spring_settings"_a)
        .def("set_max_friction", &SixDOFConstraint::SetMaxFriction, "axis"_a, "friction"_a,
            "Set the max friction for each axis")
        .def("get_max_friction", &SixDOFConstraint::GetMaxFriction, "axis"_a)
        .def("get_rotation_in_constraint_space", &SixDOFConstraint::GetRotationInConstraintSpace,
            "Get rotation of constraint in constraint space")
        .def("get_motor_settings", nb::overload_cast<SixDOFConstraint::EAxis>(&SixDOFConstraint::GetMotorSettings), "axis"_a,
            "Motor settings")
        .def("set_motor_state", &SixDOFConstraint::SetMotorState, "axis"_a, "state"_a,
            "Motor controls.\n"
            "Translation motors work in constraint space of body 1.\n"
            "Rotation motors work in constraint space of body 2 (!).")
        .def("get_motor_state", &SixDOFConstraint::GetMotorState, "axis"_a)
        .def("get_target_velocity_cs", &SixDOFConstraint::GetTargetVelocityCS,
            "Set the target velocity in body 1 constraint space")
        .def("set_target_velocity_cs", &SixDOFConstraint::SetTargetVelocityCS, "velocity"_a)
        .def("set_target_angular_velocity_cs", &SixDOFConstraint::SetTargetAngularVelocityCS, "angular_velocity"_a,
            "Set the target angular velocity in body 2 constraint space (!)")
        .def("get_target_angular_velocity_cs", &SixDOFConstraint::GetTargetAngularVelocityCS)
        .def("get_target_position_cs", &SixDOFConstraint::GetTargetPositionCS,
            "Set the target position in body 1 constraint space")
        .def("set_target_position_cs", &SixDOFConstraint::SetTargetPositionCS, "position"_a)
        .def("set_target_orientation_cs", &SixDOFConstraint::SetTargetOrientationCS, "orientation"_a,
            "Set the target orientation in body 1 constraint space")
        .def("get_target_orientation_cs", &SixDOFConstraint::GetTargetOrientationCS)
        .def("set_target_orientation_bs", &SixDOFConstraint::SetTargetOrientationBS, "orientation"_a,
            "Set the target orientation in body space (R2 = R1 * inOrientation, where R1 and R2 are the world space rotations for body 1 and 2).\n"
            "Solve: R2 * ConstraintToBody2 = R1 * ConstraintToBody1 * q (see SwingTwistConstraint::GetSwingTwist) and R2 = R1 * inOrientation for q.")
        .def("get_total_lambda_position", &SixDOFConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_rotation", &SixDOFConstraint::GetTotalLambdaRotation)
        .def("get_total_lambda_motor_translation", &SixDOFConstraint::GetTotalLambdaMotorTranslation)
        .def("get_total_lambda_motor_rotation", &SixDOFConstraint::GetTotalLambdaMotorRotation);
}