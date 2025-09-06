#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/RotationEulerConstraintPart.h>

void BindRotationEulerConstraintPart(nb::module_ &m) {
    nb::class_<RotationEulerConstraintPart> rotationEulerConstraintPartCls(m, "RotationEulerConstraintPart",
        "Constrains rotation around all axis so that only translation is allowed\n"
        "Based on: \"Constraints Derivation for Rigid Body Simulation in 3D\" - Daniel Chappuis, section 2.5.1\n"
        "Constraint equation (eq 129):\n"
        "\\f[C = \\begin{bmatrix}\\Delta\\theta_x, \\Delta\\theta_y, \\Delta\\theta_z\\end{bmatrix}\\f]\n"
        "Jacobian (eq 131):\n"
        "\\f[J = \\begin{bmatrix}0 & -E & 0 & E\\end{bmatrix}\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "delta_theta_* = difference in rotation between initial rotation of bodies 1 and 2.\\n\n"
        "x1, x2 = center of mass for the bodies.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "b = velocity bias.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.\\n\n"
        "E = identity matrix.\\n");
    rotationEulerConstraintPartCls
        .def_static("get_inv_initial_orientation", &RotationEulerConstraintPart::sGetInvInitialOrientation, "body1"_a, "body2"_a,
            "Return inverse of initial rotation from body 1 to body 2 in body 1 space")
        .def_static("get_inv_initial_orientation_xy", &RotationEulerConstraintPart::sGetInvInitialOrientationXY, "axis_x1"_a, "axis_y1"_a, "axis_x2"_a, "axis_y2"_a,
            "@brief Return inverse of initial rotation from body 1 to body 2 in body 1 space.\n"
            "Args:\n"
            "    axis_x1 (Vec3): Reference axis X for body 1.\n"
            "    axis_y1 (Vec3): Reference axis Y for body 1.\n"
            "    axis_x2 (Vec3): Reference axis X for body 2.\n"
            "    axis_y2 (Vec3): Reference axis Y for body 2.")
        .def_static("get_inv_initial_orientation_xz", &RotationEulerConstraintPart::sGetInvInitialOrientationXZ, "axis_x1"_a, "axis_z1"_a, "axis_x2"_a, "axis_z2"_a,
            "@brief Return inverse of initial rotation from body 1 to body 2 in body 1 space.\n"
            "Args:\n"
            "    axis_x1 (Vec3): Reference axis X for body 1.\n"
            "    axis_z1 (Vec3): Reference axis Z for body 1.\n"
            "    axis_x2 (Vec3): Reference axis X for body 2.\n"
            "    axis_z2 (Vec3): Reference axis Z for body 2.")
        .def("calculate_constraint_properties", &RotationEulerConstraintPart::CalculateConstraintProperties, "body1"_a, "rotation1"_a, "body2"_a, "rotation2"_a,
            "Calculate properties used during the functions below")
        .def("deactivate", &RotationEulerConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &RotationEulerConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &RotationEulerConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses")
        .def("solve_velocity_constraint", &RotationEulerConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.")
        .def("solve_position_constraint", &RotationEulerConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "inv_initial_orientation"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.")
        .def("get_total_lambda", &RotationEulerConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("save_state", &RotationEulerConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &RotationEulerConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}