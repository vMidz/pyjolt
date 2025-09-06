#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/RackAndPinionConstraintPart.h>

void BindRackAndPinionConstraintPart(nb::module_ &m) {
    nb::class_<RackAndPinionConstraintPart> rackAndPinionConstraintPartCls(m, "RackAndPinionConstraintPart",
        "Constraint that constrains a rotation to a translation\n"
        "Constraint equation:\n"
        "C = Theta(t) - r d(t)\n"
        "Derivative:\n"
        "d/dt C = 0\n"
        "<=> w1 . a - r v2 . b = 0\n"
        "Jacobian:\n"
        "\\f[J = \\begin{bmatrix}0 & a^T & -r b^T & 0\\end{bmatrix}\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "a = axis around which body 1 rotates (normalized).\\n\n"
        "b = axis along which body 2 slides (normalized).\\n\n"
        "Theta(t) = rotation around a of body 1.\\n\n"
        "d(t) = distance body 2 slides.\\n\n"
        "r = ratio between rotation and translation.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.");
    rackAndPinionConstraintPartCls
        .def("calculate_constraint_properties", &RackAndPinionConstraintPart::CalculateConstraintProperties, "body1"_a, "world_space_hinge_axis"_a, "body2"_a, "world_space_slider_axis"_a, "ratio"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_hinge_axis (Vec3): The axis around which body 1 rotates.\n"
            "    world_space_slider_axis (Vec3): The axis along which body 2 slides.\n"
            "    ratio (float): The ratio between rotation and translation.")
        .def("deactivate", &RackAndPinionConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &RackAndPinionConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &RackAndPinionConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    warm_start_impulse_ratio (float): Ratio of new step to old time step (dt_new / dt_old) for scaling the lagrange multiplier of the previous frame.")
        .def("solve_velocity_constraint", &RackAndPinionConstraintPart::SolveVelocityConstraint, "body1"_a, "world_space_hinge_axis"_a, "body2"_a, "world_space_slider_axis"_a, "ratio"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_hinge_axis (Vec3): The axis around which body 1 rotates.\n"
            "    world_space_slider_axis (Vec3): The axis along which body 2 slides.\n"
            "    ratio (float): The ratio between rotation and translation.")
        .def("get_total_lambda", &RackAndPinionConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("solve_position_constraint", &RackAndPinionConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "c"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) == 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("save_state", &RackAndPinionConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &RackAndPinionConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}