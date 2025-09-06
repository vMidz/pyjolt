#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/HingeRotationConstraintPart.h>

void BindHingeRotationConstraintPart(nb::module_ &m) {
    nb::class_<HingeRotationConstraintPart> hingeRotationConstraintPartCls(m, "HingeRotationConstraintPart",
        "/**\n"
        "    Constrains rotation around 2 axis so that it only allows rotation around 1 axis\n"
        "    Based on: \"Constraints Derivation for Rigid Body Simulation in 3D\" - Daniel Chappuis, section 2.4.1\n"
        "    Constraint equation (eq 87):\n"
        "    \\f[C = \\begin{bmatrix}a_1 \\cdot b_2 \\ a_1 \\cdot c_2\\end{bmatrix}\\f]\n"
        "    Jacobian (eq 90):\n"
        "    \\f[J = \\begin{bmatrix}\n"
        "    0   & -b_2 \\times a_1   & 0     & b_2 \\times a_1    \\\n"
        "    0   & -c_2 \\times a_1   & 0     & c2 \\times a_1\n"
        "    \\end{bmatrix}\\f]\n"
        "    Used terms (here and below, everything in world space):\\n\n"
        "    a1 = hinge axis on body 1.\\n\n"
        "    b2, c2 = axis perpendicular to hinge axis on body 2.\\n\n"
        "    x1, x2 = center of mass for the bodies.\\n\n"
        "    v = [v1, w1, v2, w2].\\n\n"
        "    v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "    w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "    M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "    \\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "    b = velocity bias.\\n\n"
        "    \\f$\\beta\\f$ = baumgarte constant.\\n\n"
        "    E = identity matrix.\n"
        "**/");
    hingeRotationConstraintPartCls
        .def("calculate_constraint_properties", &HingeRotationConstraintPart::CalculateConstraintProperties, "body1"_a, "rotation1"_a, "world_space_hinge_axis1"_a, "body2"_a, "rotation2"_a, "world_space_hinge_axis2"_a,
            "Calculate properties used during the functions below")
        .def("deactivate", &HingeRotationConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("warm_start", &HingeRotationConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses")
        .def("solve_velocity_constraint", &HingeRotationConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.")
        .def("solve_position_constraint", &HingeRotationConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.")
        .def("get_total_lambda", &HingeRotationConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("save_state", &HingeRotationConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &HingeRotationConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}