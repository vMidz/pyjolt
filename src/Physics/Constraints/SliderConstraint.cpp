#include "Common.h"
#include <Jolt/Physics/Constraints/SliderConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindSliderConstraint(nb::module_ &m) {
    nb::class_<SliderConstraintSettings, TwoBodyConstraintSettings> sliderConstraintSettingsCls(m, "SliderConstraintSettings",
        "Slider constraint settings, used to create a slider constraint",
        nb::is_final());
    sliderConstraintSettingsCls
        .def("save_binary_state", &SliderConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &SliderConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint.\n"
            "Note that the rotation constraint will be solved from body 1. This means that if body 1 and body 2 have different masses / inertias (kinematic body = infinite mass / inertia), body 1 should be the heaviest body.")
        .def("set_slider_axis", &SliderConstraintSettings::SetSliderAxis, "slider_axis"_a,
            "Simple way of setting the slider and normal axis in world space (assumes the bodies are already oriented correctly when the constraint is created)")
        .def_rw("space", &SliderConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("auto_detect_point", &SliderConstraintSettings::mAutoDetectPoint,
            "When mSpace is WorldSpace mPoint1 and mPoint2 can be automatically calculated based on the positions of the bodies when the constraint is created (the current relative position/orientation is chosen as the '0' position). Set this to false if you want to supply the attachment points yourself.")
        .def_rw("point1", &SliderConstraintSettings::mPoint1,
            "Body 1 constraint reference frame (space determined by mSpace).\n"
            "Slider axis is the axis along which movement is possible (direction), normal axis is a perpendicular vector to define the frame.")
        .def_rw("slider_axis1", &SliderConstraintSettings::mSliderAxis1)
        .def_rw("normal_axis1", &SliderConstraintSettings::mNormalAxis1)
        .def_rw("point2", &SliderConstraintSettings::mPoint2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("slider_axis2", &SliderConstraintSettings::mSliderAxis2)
        .def_rw("normal_axis2", &SliderConstraintSettings::mNormalAxis2)
        .def_rw("limits_min", &SliderConstraintSettings::mLimitsMin,
            "When the bodies move so that mPoint1 coincides with mPoint2 the slider position is defined to be 0, movement will be limited between [mLimitsMin, mLimitsMax] where mLimitsMin e [-inf, 0] and mLimitsMax e [0, inf]")
        .def_rw("limits_max", &SliderConstraintSettings::mLimitsMax)
        .def_rw("limits_spring_settings", &SliderConstraintSettings::mLimitsSpringSettings,
            "When enabled, this makes the limits soft. When the constraint exceeds the limits, a spring force will pull it back.")
        .def_rw("max_friction_force", &SliderConstraintSettings::mMaxFrictionForce,
            "Maximum amount of friction force to apply (N) when not driven by a motor.")
        .def_rw("motor_settings", &SliderConstraintSettings::mMotorSettings,
            "In case the constraint is powered, this determines the motor settings around the sliding axis");

    nb::class_<SliderConstraint, TwoBodyConstraint> sliderConstraintCls(m, "SliderConstraint",
        "A slider constraint allows movement in only 1 axis (and no rotation). Also known as a prismatic constraint.",
        nb::is_final());
    sliderConstraintCls
        .def(nb::init<Body &, Body &, const SliderConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct slider constraint")
        .def("get_sub_type", &SliderConstraint::GetSubType)
        .def("notify_shape_changed", &SliderConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &SliderConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &SliderConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &SliderConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &SliderConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &SliderConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &SliderConstraint::DrawConstraint, "renderer"_a)
        .def("draw_constraint_limits", &SliderConstraint::DrawConstraintLimits, "renderer"_a)
        .def("save_state", &SliderConstraint::SaveState, "stream"_a)
        .def("restore_state", &SliderConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &SliderConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &SliderConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &SliderConstraint::GetConstraintToBody2Matrix)
        .def("get_current_position", &SliderConstraint::GetCurrentPosition,
            "Get the current distance from the rest position")
        .def("set_max_friction_force", &SliderConstraint::SetMaxFrictionForce, "friction_force"_a,
            "Friction control")
        .def("get_max_friction_force", &SliderConstraint::GetMaxFrictionForce)
        .def("get_motor_settings", nb::overload_cast<>(&SliderConstraint::GetMotorSettings),
            "Motor settings")
        .def("set_motor_state", &SliderConstraint::SetMotorState, "state"_a)
        .def("get_motor_state", &SliderConstraint::GetMotorState)
        .def("set_target_velocity", &SliderConstraint::SetTargetVelocity, "velocity"_a)
        .def("get_target_velocity", &SliderConstraint::GetTargetVelocity)
        .def("set_target_position", &SliderConstraint::SetTargetPosition, "position"_a)
        .def("get_target_position", &SliderConstraint::GetTargetPosition)
        .def("set_limits", &SliderConstraint::SetLimits, "limits_min"_a, "limits_max"_a,
            "Update the limits of the slider constraint (see SliderConstraintSettings)")
        .def("get_limits_min", &SliderConstraint::GetLimitsMin)
        .def("get_limits_max", &SliderConstraint::GetLimitsMax)
        .def("has_limits", &SliderConstraint::HasLimits)
        .def("get_limits_spring_settings", nb::overload_cast<>(&SliderConstraint::GetLimitsSpringSettings),
            "Update the limits spring settings")
        .def("set_limits_spring_settings", &SliderConstraint::SetLimitsSpringSettings, "limits_spring_settings"_a)
        .def("get_total_lambda_position", &SliderConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_position_limits", &SliderConstraint::GetTotalLambdaPositionLimits)
        .def("get_total_lambda_rotation", &SliderConstraint::GetTotalLambdaRotation)
        .def("get_total_lambda_motor", &SliderConstraint::GetTotalLambdaMotor);
}