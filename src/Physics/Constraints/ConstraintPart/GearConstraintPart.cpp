#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/GearConstraintPart.h>

void BindGearConstraintPart(nb::module_ &m) {
    nb::class_<GearConstraintPart> gearConstraintPartCls(m, "GearConstraintPart",
        "Constraint that constrains two rotations using a gear (rotating in opposite direction)\n"
        "Constraint equation:\n"
        "C = Rotation1(t) + r Rotation2(t)\n"
        "Derivative:\n"
        "d/dt C = 0\n"
        "<=> w1 . a + r w2 . b = 0\n"
        "Jacobian:\n"
        "\\f[J = \\begin{bmatrix}0 & a^T & 0 & r b^T\\end{bmatrix}\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "a = axis around which body 1 rotates (normalized).\\n\n"
        "b = axis along which body 2 slides (normalized).\\n\n"
        "Rotation1(t) = rotation around a of body 1.\\n\n"
        "Rotation2(t) = rotation around b of body 2.\\n\n"
        "r = ratio between rotation for body 1 and 2.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.");
    gearConstraintPartCls
        .def("calculate_constraint_properties", &GearConstraintPart::CalculateConstraintProperties, "body1"_a, "world_space_hinge_axis1"_a, "body2"_a, "world_space_hinge_axis2"_a, "ratio"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_hinge_axis1 (Vec3): The axis around which body 1 rotates.\n"
            "    world_space_hinge_axis2 (Vec3): The axis around which body 2 rotates.\n"
            "    ratio (float): The ratio between rotation and translation.")
        .def("deactivate", &GearConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &GearConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &GearConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    warm_start_impulse_ratio (float): Ratio of new step to old time step (dt_new / dt_old) for scaling the lagrange multiplier of the previous frame.")
        .def("solve_velocity_constraint", &GearConstraintPart::SolveVelocityConstraint, "body1"_a, "world_space_hinge_axis1"_a, "body2"_a, "world_space_hinge_axis2"_a, "ratio"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_hinge_axis1 (Vec3): The axis around which body 1 rotates.\n"
            "    world_space_hinge_axis2 (Vec3): The axis around which body 2 rotates.\n"
            "    ratio (float): The ratio between rotation and translation.")
        .def("get_total_lambda", &GearConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("solve_position_constraint", &GearConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "c"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) == 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("save_state", &GearConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &GearConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}