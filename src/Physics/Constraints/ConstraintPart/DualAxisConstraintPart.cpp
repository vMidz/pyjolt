#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/DualAxisConstraintPart.h>

void BindDualAxisConstraintPart(nb::module_ &m) {
    nb::class_<DualAxisConstraintPart> dualAxisConstraintPartCls(m, "DualAxisConstraintPart",
        "/**\n"
        "    Constrains movement on 2 axis\n"
        "    @see \"Constraints Derivation for Rigid Body Simulation in 3D\" - Daniel Chappuis, section 2.3.1\n"
        "    Constraint equation (eq 51):\n"
        "    \\f[C = \\begin{bmatrix} (p_2 - p_1) \\cdot n_1 \\ (p_2 - p_1) \\cdot n_2\\end{bmatrix}\\f]\n"
        "    Jacobian (transposed) (eq 55):\n"
        "    \\f[J^T = \\begin{bmatrix}\n"
        "    -n_1                    & -n_2                  \\\n"
        "    -(r_1 + u) \\times n_1   & -(r_1 + u) \\times n_2 \\\n"
        "    n_1                     & n_2                   \\\n"
        "    r_2 \\times n_1          & r_2 \\times n_2\n"
        "    \\end{bmatrix}\\f]\n"
        "    Used terms (here and below, everything in world space):\\n\n"
        "    n1, n2 = constraint axis (normalized).\\n\n"
        "    p1, p2 = constraint points.\\n\n"
        "    r1 = p1 - x1.\\n\n"
        "    r2 = p2 - x2.\\n\n"
        "    u = x2 + r2 - x1 - r1 = p2 - p1.\\n\n"
        "    x1, x2 = center of mass for the bodies.\\n\n"
        "    v = [v1, w1, v2, w2].\\n\n"
        "    v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "    w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "    M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "    \\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "    b = velocity bias.\\n\n"
        "    \\f$\\beta\\f$ = baumgarte constant.\n"
        "**/");
    dualAxisConstraintPartCls
        .def("calculate_constraint_properties", &DualAxisConstraintPart::CalculateConstraintProperties, "body1"_a, "rotation1"_a, "r1_plus_u"_a, "body2"_a, "rotation2"_a, "r2"_a, "n1"_a, "n2"_a,
            "Calculate properties used during the functions below\n"
            "All input vectors are in world space")
        .def("deactivate", &DualAxisConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &DualAxisConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &DualAxisConstraintPart::WarmStart, "body1"_a, "body2"_a, "n1"_a, "n2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses\n"
            "All input vectors are in world space")
        .def("solve_velocity_constraint", &DualAxisConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a, "n1"_a, "n2"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "All input vectors are in world space")
        .def("solve_position_constraint", &DualAxisConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "u"_a, "n1"_a, "n2"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.\n"
            "All input vectors are in world space")
        .def("set_total_lambda", &DualAxisConstraintPart::SetTotalLambda, "lambda_"_a,
            "Override total lagrange multiplier, can be used to set the initial value for warm starting")
        .def("get_total_lambda", &DualAxisConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("save_state", &DualAxisConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &DualAxisConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}