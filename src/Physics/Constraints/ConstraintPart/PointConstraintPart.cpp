#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/PointConstraintPart.h>

void BindPointConstraintPart(nb::module_ &m) {
    nb::class_<PointConstraintPart> pointConstraintPartCls(m, "PointConstraintPart",
        "Constrains movement along 3 axis\n"
        "@see \"Constraints Derivation for Rigid Body Simulation in 3D\" - Daniel Chappuis, section 2.2.1\n"
        "Constraint equation (eq 45):\n"
        "\\f[C = p_2 - p_1\\f]\n"
        "Jacobian (transposed) (eq 47):\n"
        "\\f[J^T = \\begin{bmatrix}-E & r1x & E & -r2x^T\\end{bmatrix}\n"
        "= \\begin{bmatrix}-E^T \\ r1x^T \\ E^T \\ -r2x^T\\end{bmatrix}\n"
        "= \\begin{bmatrix}-E \\ -r1x \\ E \\ r2x\\end{bmatrix}\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "p1, p2 = constraint points.\\n\n"
        "r1 = p1 - x1.\\n\n"
        "r2 = p2 - x2.\\n\n"
        "r1x = 3x3 matrix for which r1x v = r1 x v (cross product).\\n\n"
        "x1, x2 = center of mass for the bodies.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "b = velocity bias.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.\\n\n"
        "E = identity matrix.");
    pointConstraintPartCls
        .def("calculate_constraint_properties", &PointConstraintPart::CalculateConstraintProperties, "body1"_a, "rotation1"_a, "r1"_a, "body2"_a, "rotation2"_a, "r2"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    rotation1 (Mat44): The 3x3 rotation matrix for body 1 (translation part is ignored).\n"
            "    rotation2 (Mat44): The 3x3 rotation matrix for body 2 (translation part is ignored).\n"
            "    r1 (Vec3): Local space vector from center of mass to constraint point for body 1.\n"
            "    r2 (Vec3): Local space vector from center of mass to constraint point for body 2.")
        .def("deactivate", &PointConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &PointConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &PointConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    warm_start_impulse_ratio (float): Ratio of new step to old time step (dt_new / dt_old) for scaling the lagrange multiplier of the previous frame.")
        .def("solve_velocity_constraint", &PointConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.")
        .def("solve_position_constraint", &PointConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("get_total_lambda", &PointConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("save_state", &PointConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &PointConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}