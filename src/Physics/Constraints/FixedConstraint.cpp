#include "Common.h"
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindFixedConstraint(nb::module_ &m) {
    nb::class_<FixedConstraintSettings, TwoBodyConstraintSettings> fixedConstraintSettingsCls(m, "FixedConstraintSettings",
        "Fixed constraint settings, used to create a fixed constraint",
        nb::is_final());
    fixedConstraintSettingsCls
        .def("save_binary_state", &FixedConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &FixedConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint")
        .def_rw("space", &FixedConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("auto_detect_point", &FixedConstraintSettings::mAutoDetectPoint,
            "When mSpace is WorldSpace mPoint1 and mPoint2 can be automatically calculated based on the positions of the bodies when the constraint is created (they will be fixated in their current relative position/orientation). Set this to false if you want to supply the attachment points yourself.")
        .def_rw("point1", &FixedConstraintSettings::mPoint1,
            "Body 1 constraint reference frame (space determined by mSpace)")
        .def_rw("axis_x1", &FixedConstraintSettings::mAxisX1)
        .def_rw("axis_y1", &FixedConstraintSettings::mAxisY1)
        .def_rw("point2", &FixedConstraintSettings::mPoint2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("axis_x2", &FixedConstraintSettings::mAxisX2)
        .def_rw("axis_y2", &FixedConstraintSettings::mAxisY2);

    nb::class_<FixedConstraint, TwoBodyConstraint> fixedConstraintCls(m, "FixedConstraint",
        "A fixed constraint welds two bodies together removing all degrees of freedom between them.\n"
        "This variant uses Euler angles for the rotation constraint.",
        nb::is_final());
    fixedConstraintCls
        .def(nb::init<Body &, Body &, const FixedConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Constructor")
        .def("get_sub_type", &FixedConstraint::GetSubType)
        .def("notify_shape_changed", &FixedConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &FixedConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &FixedConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &FixedConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &FixedConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &FixedConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &FixedConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &FixedConstraint::SaveState, "stream"_a)
        .def("restore_state", &FixedConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &FixedConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &FixedConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &FixedConstraint::GetConstraintToBody2Matrix)
        .def("get_total_lambda_position", &FixedConstraint::GetTotalLambdaPosition,
            "///@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)")
        .def("get_total_lambda_rotation", &FixedConstraint::GetTotalLambdaRotation);
}