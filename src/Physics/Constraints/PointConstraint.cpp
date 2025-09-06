#include "Common.h"
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindPointConstraint(nb::module_ &m) {
    nb::class_<PointConstraintSettings, TwoBodyConstraintSettings> pointConstraintSettingsCls(m, "PointConstraintSettings",
        "Point constraint settings, used to create a point constraint",
        nb::is_final());
    pointConstraintSettingsCls
        .def("save_binary_state", &PointConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &PointConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &PointConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("point1", &PointConstraintSettings::mPoint1,
            "Body 1 constraint position (space determined by mSpace).")
        .def_rw("point2", &PointConstraintSettings::mPoint2,
            "Body 2 constraint position (space determined by mSpace).\n"
            "Note: Normally you would set mPoint1 = mPoint2 if the bodies are already placed how you want to constrain them (if mSpace = world space).");

    nb::class_<PointConstraint, TwoBodyConstraint> pointConstraintCls(m, "PointConstraint",
        "A point constraint constrains 2 bodies on a single point (removing 3 degrees of freedom)",
        nb::is_final());
    pointConstraintCls
        .def(nb::init<Body &, Body &, const PointConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct point constraint")
        .def("get_sub_type", &PointConstraint::GetSubType)
        .def("notify_shape_changed", &PointConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &PointConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &PointConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &PointConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &PointConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &PointConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &PointConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &PointConstraint::SaveState, "stream"_a)
        .def("restore_state", &PointConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &PointConstraint::GetConstraintSettings)
        .def("set_point1", &PointConstraint::SetPoint1, "space"_a, "point1"_a,
            "Update the attachment point for body 1")
        .def("set_point2", &PointConstraint::SetPoint2, "space"_a, "point2"_a,
            "Update the attachment point for body 2")
        .def("get_local_space_point1", &PointConstraint::GetLocalSpacePoint1,
            "Get the attachment point for body 1 relative to body 1 COM (transform by Body::GetCenterOfMassTransform to take to world space)")
        .def("get_local_space_point2", &PointConstraint::GetLocalSpacePoint2,
            "Get the attachment point for body 2 relative to body 2 COM (transform by Body::GetCenterOfMassTransform to take to world space)")
        .def("get_constraint_to_body1_matrix", &PointConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &PointConstraint::GetConstraintToBody2Matrix)
        .def("get_total_lambda_position", &PointConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear impulse applied to satisfy the constraint)");
}