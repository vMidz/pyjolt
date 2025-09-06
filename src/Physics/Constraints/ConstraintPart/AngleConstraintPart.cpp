#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/AngleConstraintPart.h>

void BindAngleConstraintPart(nb::module_ &m) {
    nb::class_<AngleConstraintPart> angleConstraintPartCls(m, "AngleConstraintPart",
        "Constraint that constrains rotation along 1 axis\n"
        "Based on: \"Constraints Derivation for Rigid Body Simulation in 3D\" - Daniel Chappuis, see section 2.4.5\n"
        "Constraint equation (eq 108):\n"
        "\\f[C = \\theta(t) - \\theta_{min}\\f]\n"
        "Jacobian (eq 109):\n"
        "\\f[J = \\begin{bmatrix}0 & -a^T & 0 & a^T\\end{bmatrix}\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "a = axis around which rotation is constrained (normalized).\\n\n"
        "x1, x2 = center of mass for the bodies.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "b = velocity bias.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.");
    angleConstraintPartCls
        .def("calculate_constraint_properties", &AngleConstraintPart::CalculateConstraintProperties, "body1"_a, "body2"_a, "world_space_axis"_a, "bias"_a = 0.0f,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): The axis of rotation along which the constraint acts (normalized)\n"
            "Set the following terms to zero if you don't want to drive the constraint to zero with a spring:.\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.")
        .def("calculate_constraint_properties_with_frequency_and_damping", &AngleConstraintPart::CalculateConstraintPropertiesWithFrequencyAndDamping, "delta_time"_a, "body1"_a, "body2"_a, "world_space_axis"_a, "bias"_a, "c"_a, "frequency"_a, "damping"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    delta_time (float): Time step.\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): The axis of rotation along which the constraint acts (normalized)\n"
            "Set the following terms to zero if you don't want to drive the constraint to zero with a spring:.\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    frequency (float): Oscillation frequency (Hz).\n"
            "    damping (float): Damping factor (0 = no damping, 1 = critical damping).")
        .def("calculate_constraint_properties_with_stiffness_and_damping", &AngleConstraintPart::CalculateConstraintPropertiesWithStiffnessAndDamping, "delta_time"_a, "body1"_a, "body2"_a, "world_space_axis"_a, "bias"_a, "c"_a, "stiffness"_a, "damping"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    delta_time (float): Time step.\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): The axis of rotation along which the constraint acts (normalized)\n"
            "Set the following terms to zero if you don't want to drive the constraint to zero with a spring:.\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    stiffness (float): Spring stiffness k.\n"
            "    damping (float): Spring damping coefficient c.")
        .def("calculate_constraint_properties_with_settings", &AngleConstraintPart::CalculateConstraintPropertiesWithSettings, "delta_time"_a, "body1"_a, "body2"_a, "world_space_axis"_a, "bias"_a, "c"_a, "spring_settings"_a,
            "Selects one of the above functions based on the spring settings")
        .def("deactivate", &AngleConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &AngleConstraintPart::IsActive,
            "Check if constraint is active")
        .def("warm_start", &AngleConstraintPart::WarmStart, "body1"_a, "body2"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    warm_start_impulse_ratio (float): Ratio of new step to old time step (dt_new / dt_old) for scaling the lagrange multiplier of the previous frame.")
        .def("solve_velocity_constraint", &AngleConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a, "world_space_axis"_a, "min_lambda"_a, "max_lambda"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): The axis of rotation along which the constraint acts (normalized).\n"
            "    min_lambda (float): Minimum angular impulse to apply (N m s).\n"
            "    max_lambda (float): Maximum angular impulse to apply (N m s).")
        .def("get_total_lambda", &AngleConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("solve_position_constraint", &AngleConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "c"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) == 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("save_state", &AngleConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &AngleConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}
