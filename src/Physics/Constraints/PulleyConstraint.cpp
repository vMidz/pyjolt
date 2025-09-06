#include "Common.h"
#include <Jolt/Physics/Constraints/PulleyConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindPulleyConstraint(nb::module_ &m) {
    nb::class_<PulleyConstraintSettings, TwoBodyConstraintSettings> pulleyConstraintSettingsCls(m, "PulleyConstraintSettings",
        "Pulley constraint settings, used to create a pulley constraint.\n"
        "A pulley connects two bodies via two fixed world points to each other similar to a distance constraint.\n"
        "We define Length1 = |BodyPoint1 - FixedPoint1| where Body1 is a point on body 1 in world space and FixedPoint1 a fixed point in world space\n"
        "Length2 = |BodyPoint2 - FixedPoint2|\n"
        "The constraint keeps the two line segments constrained so that\n"
        "MinDistance <= Length1 + Ratio * Length2 <= MaxDistance",
        nb::is_final());
    pulleyConstraintSettingsCls
        .def("save_binary_state", &PulleyConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &PulleyConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &PulleyConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, specified properties below should be in the specified space")
        .def_rw("body_point1", &PulleyConstraintSettings::mBodyPoint1,
            "Body 1 constraint attachment point (space determined by mSpace).")
        .def_rw("fixed_point1", &PulleyConstraintSettings::mFixedPoint1,
            "Fixed world point to which body 1 is connected (always world space)")
        .def_rw("body_point2", &PulleyConstraintSettings::mBodyPoint2,
            "Body 2 constraint attachment point (space determined by mSpace)")
        .def_rw("fixed_point2", &PulleyConstraintSettings::mFixedPoint2,
            "Fixed world point to which body 2 is connected (always world space)")
        .def_rw("ratio", &PulleyConstraintSettings::mRatio,
            "Ratio between the two line segments (see formula above), can be used to create a block and tackle")
        .def_rw("min_length", &PulleyConstraintSettings::mMinLength,
            "The minimum length of the line segments (see formula above), use -1 to calculate the length based on the positions of the objects when the constraint is created.")
        .def_rw("max_length", &PulleyConstraintSettings::mMaxLength,
            "The maximum length of the line segments (see formula above), use -1 to calculate the length based on the positions of the objects when the constraint is created.");

    nb::class_<PulleyConstraint, TwoBodyConstraint> pulleyConstraintCls(m, "PulleyConstraint",
        "A pulley constraint.",
        nb::is_final());
    pulleyConstraintCls
        .def(nb::init<Body &, Body &, const PulleyConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct distance constraint")
        .def("get_sub_type", &PulleyConstraint::GetSubType)
        .def("notify_shape_changed", &PulleyConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &PulleyConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &PulleyConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &PulleyConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &PulleyConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &PulleyConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &PulleyConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &PulleyConstraint::SaveState, "stream"_a)
        .def("restore_state", &PulleyConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &PulleyConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &PulleyConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &PulleyConstraint::GetConstraintToBody2Matrix)
        .def("set_length", &PulleyConstraint::SetLength, "min_length"_a, "max_length"_a,
            "Update the minimum and maximum length for the constraint")
        .def("get_min_length", &PulleyConstraint::GetMinLength)
        .def("get_max_length", &PulleyConstraint::GetMaxLength)
        .def("get_current_length", &PulleyConstraint::GetCurrentLength,
            "Get the current length of both segments (multiplied by the ratio for segment 2)")
        .def("get_total_lambda_position", &PulleyConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear impulse applied to satisfy the constraint)");
}