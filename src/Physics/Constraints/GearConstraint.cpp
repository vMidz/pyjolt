#include "Common.h"
#include <Jolt/Physics/Constraints/GearConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindGearConstraint(nb::module_ &m) {
    nb::class_<GearConstraintSettings, TwoBodyConstraintSettings> gearConstraintSettingsCls(m, "GearConstraintSettings",
        "Gear constraint settings",
        nb::is_final());
    gearConstraintSettingsCls
        .def("save_binary_state", &GearConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &GearConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint.")
        .def("set_ratio", &GearConstraintSettings::SetRatio, "num_teeth_gear1"_a, "num_teeth_gear2"_a,
            "Defines the ratio between the rotation of both gears\n"
            "The ratio is defined as: Gear1Rotation(t) = -ratio * Gear2Rotation(t).\n"
            "Args:\n"
            "    num_teeth_gear1 (int): Number of teeth that body 1 has.\n"
            "    num_teeth_gear2 (int): Number of teeth that body 2 has.")
        .def_rw("space", &GearConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("hinge_axis1", &GearConstraintSettings::mHingeAxis1,
            "Body 1 constraint reference frame (space determined by mSpace).")
        .def_rw("hinge_axis2", &GearConstraintSettings::mHingeAxis2,
            "Body 2 constraint reference frame (space determined by mSpace)")
        .def_rw("ratio", &GearConstraintSettings::mRatio,
            "Ratio between both gears, see SetRatio.");

    nb::class_<GearConstraint, TwoBodyConstraint> gearConstraintCls(m, "GearConstraint",
        "A gear constraint constrains the rotation of body1 to the rotation of body 2 using a gear.\n"
        "Note that this constraint needs to be used in conjunction with a two hinge constraints.",
        nb::is_final());
    gearConstraintCls
        .def(nb::init<Body &, Body &, const GearConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct gear constraint")
        .def("get_sub_type", &GearConstraint::GetSubType)
        .def("notify_shape_changed", &GearConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &GearConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &GearConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &GearConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &GearConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &GearConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &GearConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &GearConstraint::SaveState, "stream"_a)
        .def("restore_state", &GearConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &GearConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &GearConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &GearConstraint::GetConstraintToBody2Matrix)
        .def("set_constraints", &GearConstraint::SetConstraints, "gear1"_a, "gear2"_a,
            "The constraints that constrain both gears (2 hinges), optional and used to calculate the rotation error and fix numerical drift.")
        .def("get_total_lambda", &GearConstraint::GetTotalLambda,
            "///@name Get Lagrange multiplier from last physics update (the angular impulse applied to satisfy the constraint)");
}