#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/SwingTwistConstraintPart.h>

void BindSwingTwistConstraintPart(nb::module_ &m) {
    nb::enum_<ESwingType>(m, "ESwingType",
        "How the swing limit behaves")
        .value("CONE", ESwingType::Cone,
            "Swing is limited by a cone shape, note that this cone starts to deform for larger swing angles. Cone limits only support limits that are symmetric around 0.")
        .value("PYRAMID", ESwingType::Pyramid,
            "Swing is limited by a pyramid shape, note that this pyramid starts to deform for larger swing angles.");
    nb::class_<SwingTwistConstraintPart> swingTwistConstraintPartCls(m, "SwingTwistConstraintPart",
        "Quaternion based constraint that decomposes the rotation in constraint space in swing and twist: q = q_swing * q_twist\n"
        "where q_swing.x = 0 and where q_twist.y = q_twist.z = 0\n"
        "- Rotation around the twist (x-axis) is within [inTwistMinAngle, inTwistMaxAngle].\n"
        "- Rotation around the swing axis (y and z axis) are limited to an ellipsoid in quaternion space formed by the equation:\n"
        "(q_swing.y / sin(inSwingYHalfAngle / 2))^2 + (q_swing.z / sin(inSwingZHalfAngle / 2))^2 <= 1\n"
        "Which roughly corresponds to an elliptic cone shape with major axis (inSwingYHalfAngle, inSwingZHalfAngle).\n"
        "In case inSwingYHalfAngle = 0, the rotation around Y will be constrained to 0 and the rotation around Z\n"
        "will be constrained between [-inSwingZHalfAngle, inSwingZHalfAngle]. Vice versa if inSwingZHalfAngle = 0.");
    swingTwistConstraintPartCls
        .def("set_swing_type", &SwingTwistConstraintPart::SetSwingType, "swing_type"_a,
            "Override the swing type")
        .def("get_swing_type", &SwingTwistConstraintPart::GetSwingType,
            "Get the swing type for this part")
        .def("set_limits", &SwingTwistConstraintPart::SetLimits, "twist_min_angle"_a, "twist_max_angle"_a, "swing_y_min_angle"_a, "swing_y_max_angle"_a, "swing_z_min_angle"_a, "swing_z_max_angle"_a,
            "Set limits for this constraint (see description above for parameters)")
        .def_static("distance_to_min_shorter", &SwingTwistConstraintPart::sDistanceToMinShorter, "delta_min"_a, "delta_max"_a,
            "Helper function to determine if we're clamped against the min or max limit")
        .def("clamp_swing_twist", [](const SwingTwistConstraintPart &self, Quat ioSwing, Quat ioTwist) {
            uint outClampedAxis;
            Quat swing_copy = ioSwing;
            Quat twist_copy = ioTwist;
            self.ClampSwingTwist(swing_copy, twist_copy, outClampedAxis);
            return nb::make_tuple(swing_copy, twist_copy, outClampedAxis);
        }, "swing"_a, "twist"_a,
            "Clamp twist and swing against the constraint limits, returns which parts were clamped (everything assumed in constraint space)\n"
            "Output [swing, twist, clamped_axis]")
        .def("calculate_constraint_properties", &SwingTwistConstraintPart::CalculateConstraintProperties, "body1"_a, "body2"_a, "constraint_rotation"_a, "constraint_to_world"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    constraint_rotation (Quat): The current rotation of the constraint in constraint space.\n"
            "    constraint_to_world (Quat): Rotates from constraint space into world space.")
        .def("deactivate", &SwingTwistConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &SwingTwistConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &SwingTwistConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses")
        .def("solve_velocity_constraint", &SwingTwistConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.")
        .def("solve_position_constraint", &SwingTwistConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "constraint_rotation"_a, "constraint_to_body1"_a, "constraint_to_body2"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    constraint_rotation (Quat): The current rotation of the constraint in constraint space.\n"
            "    constraint_to_body1 (Quat): , inConstraintToBody2 Rotates from constraint space to body 1/2 space.\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("get_total_swing_y_lambda", &SwingTwistConstraintPart::GetTotalSwingYLambda,
            "Return lagrange multiplier for swing")
        .def("get_total_swing_z_lambda", &SwingTwistConstraintPart::GetTotalSwingZLambda)
        .def("get_total_twist_lambda", &SwingTwistConstraintPart::GetTotalTwistLambda,
            "Return lagrange multiplier for twist")
        .def("save_state", &SwingTwistConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &SwingTwistConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part")
        .def_ro_static("C_CLAMPED_TWIST_MIN", &SwingTwistConstraintPart::cClampedTwistMin,
            "Flags to indicate which axis got clamped by ClampSwingTwist")
        .def_ro_static("C_CLAMPED_TWIST_MAX", &SwingTwistConstraintPart::cClampedTwistMax)
        .def_ro_static("C_CLAMPED_SWING_Y_MIN", &SwingTwistConstraintPart::cClampedSwingYMin)
        .def_ro_static("C_CLAMPED_SWING_Y_MAX", &SwingTwistConstraintPart::cClampedSwingYMax)
        .def_ro_static("C_CLAMPED_SWING_Z_MIN", &SwingTwistConstraintPart::cClampedSwingZMin)
        .def_ro_static("C_CLAMPED_SWING_Z_MAX", &SwingTwistConstraintPart::cClampedSwingZMax);
}