#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/IndependentAxisConstraintPart.h>

void BindIndependentAxisConstraintPart(nb::module_ &m) {
    nb::class_<IndependentAxisConstraintPart> independentAxisConstraintPartCls(m, "IndependentAxisConstraintPart",
        "Constraint part to an AxisConstraintPart but both bodies have an independent axis on which the force is applied.\n"
        "Constraint equation:\n"
        "\\f[C = (x_1 + r_1 - f_1) . n_1 + r (x_2 + r_2 - f_2) \\cdot n_2\\f]\n"
        "Calculating the Jacobian:\n"
        "\\f[dC/dt = (v_1 + w_1 \\times r_1) \\cdot n_1 + (x_1 + r_1 - f_1) \\cdot d n_1/dt + r (v_2 + w_2 \\times r_2) \\cdot n_2 + r (x_2 + r_2 - f_2) \\cdot d n_2/dt\\f]\n"
        "Assuming that d n1/dt and d n2/dt are small this becomes:\n"
        "\\f[(v_1 + w_1 \\times r_1) \\cdot n_1 + r (v_2 + w_2 \\times r_2) \\cdot n_2\\f]\n"
        "\\f[= v_1 \\cdot n_1 + r_1 \\times n_1 \\cdot w_1 + r v_2 \\cdot n_2 + r r_2 \\times n_2 \\cdot w_2\\f]\n"
        "Jacobian:\n"
        "\\f[J = \\begin{bmatrix}n_1 & r_1 \\times n_1 & r n_2 & r r_2 \\times n_2\\end{bmatrix}\\f]\n"
        "Effective mass:\n"
        "\\f[K = m_1^{-1} + r_1 \\times n_1 I_1^{-1} r_1 \\times n_1 + r^2 m_2^{-1} + r^2 r_2 \\times n_2 I_2^{-1} r_2 \\times n_2\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "n1 = (x1 + r1 - f1) / |x1 + r1 - f1|, axis along which the force is applied for body 1\\n\n"
        "n2 = (x2 + r2 - f2) / |x2 + r2 - f2|, axis along which the force is applied for body 2\\n\n"
        "r = ratio how forces are applied between bodies.\\n\n"
        "x1, x2 = center of mass for the bodies.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "b = velocity bias.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.");
    independentAxisConstraintPartCls
        .def("calculate_constraint_properties", &IndependentAxisConstraintPart::CalculateConstraintProperties, "body1"_a, "body2"_a, "r1"_a, "n1"_a, "r2"_a, "n2"_a, "ratio"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    r1 (Vec3): The position on which the constraint operates on body 1 relative to COM.\n"
            "    n1 (Vec3): The world space normal in which the constraint operates for body 1.\n"
            "    r2 (Vec3): The position on which the constraint operates on body 1 relative to COM.\n"
            "    n2 (Vec3): The world space normal in which the constraint operates for body 2.\n"
            "    ratio (float): The ratio how forces are applied between bodies.")
        .def("deactivate", &IndependentAxisConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &IndependentAxisConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &IndependentAxisConstraintPart::WarmStart, "body1"_a, "body2"_a, "n1"_a, "n2"_a, "ratio"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    n1 (Vec3): The world space normal in which the constraint operates for body 1.\n"
            "    n2 (Vec3): The world space normal in which the constraint operates for body 2.\n"
            "    ratio (float): The ratio how forces are applied between bodies.\n"
            "    warm_start_impulse_ratio (float): Ratio of new step to old time step (dt_new / dt_old) for scaling the lagrange multiplier of the previous frame.")
        .def("solve_velocity_constraint", &IndependentAxisConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a, "n1"_a, "n2"_a, "ratio"_a, "min_lambda"_a, "max_lambda"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    n1 (Vec3): The world space normal in which the constraint operates for body 1.\n"
            "    n2 (Vec3): The world space normal in which the constraint operates for body 2.\n"
            "    ratio (float): The ratio how forces are applied between bodies.\n"
            "    min_lambda (float): Minimum angular impulse to apply (N m s).\n"
            "    max_lambda (float): Maximum angular impulse to apply (N m s).")
        .def("get_total_lambda", &IndependentAxisConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("solve_position_constraint", &IndependentAxisConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "n1"_a, "n2"_a, "ratio"_a, "c"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) == 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    n1 (Vec3): The world space normal in which the constraint operates for body 1.\n"
            "    n2 (Vec3): The world space normal in which the constraint operates for body 2.\n"
            "    ratio (float): The ratio how forces are applied between bodies.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("save_state", &IndependentAxisConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &IndependentAxisConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}