#include "Common.h"
#include <Jolt/Physics/Constraints/RackAndPinionConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindRackAndPinionConstraint(nb::module_ &m) {
    nb::class_<RackAndPinionConstraintSettings, TwoBodyConstraintSettings> rackAndPinionConstraintSettingsCls(m, "RackAndPinionConstraintSettings",
        "Rack and pinion constraint (slider & gear) settings",
        nb::is_final());
    rackAndPinionConstraintSettingsCls
        .def("save_binary_state", &RackAndPinionConstraintSettings::SaveBinaryState, "stream"_a)
        .def("create", &RackAndPinionConstraintSettings::Create, "body1"_a, "body2"_a, nb::rv_policy::reference,
            "Create an instance of this constraint.\n"
            "Body1 should be the pinion (gear) and body 2 the rack (slider).")
        .def("set_ratio", &RackAndPinionConstraintSettings::SetRatio, "num_teeth_rack"_a, "rack_length"_a, "num_teeth_pinion"_a,
            "Defines the ratio between the rotation of the pinion and the translation of the rack.\n"
            "The ratio is defined as: PinionRotation(t) = ratio * RackTranslation(t).\n"
            "Args:\n"
            "    num_teeth_rack (int): Number of teeth that the rack has.\n"
            "    rack_length (float): Length of the rack.\n"
            "    num_teeth_pinion (int): Number of teeth the pinion has.")
        .def_rw("space", &RackAndPinionConstraintSettings::mSpace,
            "This determines in which space the constraint is setup, all properties below should be in the specified space")
        .def_rw("hinge_axis", &RackAndPinionConstraintSettings::mHingeAxis,
            "Body 1 (pinion) constraint reference frame (space determined by mSpace).")
        .def_rw("slider_axis", &RackAndPinionConstraintSettings::mSliderAxis,
            "Body 2 (rack) constraint reference frame (space determined by mSpace)")
        .def_rw("ratio", &RackAndPinionConstraintSettings::mRatio,
            "Ratio between the rack and pinion, see SetRatio.");

    nb::class_<RackAndPinionConstraint, TwoBodyConstraint> rackAndPinionConstraintCls(m, "RackAndPinionConstraint",
        "A rack and pinion constraint constrains the rotation of body1 to the translation of body 2.\n"
        "Note that this constraint needs to be used in conjunction with a hinge constraint for body 1 and a slider constraint for body 2.",
        nb::is_final());
    rackAndPinionConstraintCls
        .def(nb::init<Body &, Body &, const RackAndPinionConstraintSettings &>(), "body1"_a, "body2"_a, "settings"_a,
            "Construct gear constraint")
        .def("get_sub_type", &RackAndPinionConstraint::GetSubType)
        .def("notify_shape_changed", &RackAndPinionConstraint::NotifyShapeChanged, "body_id"_a, "delta_com"_a)
        .def("setup_velocity_constraint", &RackAndPinionConstraint::SetupVelocityConstraint, "delta_time"_a)
        .def("reset_warm_start", &RackAndPinionConstraint::ResetWarmStart)
        .def("warm_start_velocity_constraint", &RackAndPinionConstraint::WarmStartVelocityConstraint, "warm_start_impulse_ratio"_a)
        .def("solve_velocity_constraint", &RackAndPinionConstraint::SolveVelocityConstraint, "delta_time"_a)
        .def("solve_position_constraint", &RackAndPinionConstraint::SolvePositionConstraint, "delta_time"_a, "baumgarte"_a)
        .def("draw_constraint", &RackAndPinionConstraint::DrawConstraint, "renderer"_a)
        .def("save_state", &RackAndPinionConstraint::SaveState, "stream"_a)
        .def("restore_state", &RackAndPinionConstraint::RestoreState, "stream"_a)
        .def("get_constraint_settings", &RackAndPinionConstraint::GetConstraintSettings)
        .def("get_constraint_to_body1_matrix", &RackAndPinionConstraint::GetConstraintToBody1Matrix)
        .def("get_constraint_to_body2_matrix", &RackAndPinionConstraint::GetConstraintToBody2Matrix)
        .def("set_constraints", &RackAndPinionConstraint::SetConstraints, "pinion"_a, "rack"_a,
            "The constraints that constrain the rack and pinion (a slider and a hinge), optional and used to calculate the position error and fix numerical drift.")
        .def("get_total_lambda", &RackAndPinionConstraint::GetTotalLambda,
            "@name Get Lagrange multiplier from last physics update (the linear/angular impulse applied to satisfy the constraint)");
}