#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/AxisConstraintPart.h>

void BindAxisConstraintPart(nb::module_ &m) {
    // TODO: templates
    nb::class_<AxisConstraintPart> axisConstraintPartCls(m, "AxisConstraintPart",
        "Constraint that constrains motion along 1 axis\n"
        "@see \"Constraints Derivation for Rigid Body Simulation in 3D\" - Daniel Chappuis, section 2.1.1\n"
        "(we're not using the approximation of eq 27 but instead add the U term as in eq 55)\n"
        "Constraint equation (eq 25):\n"
        "\\f[C = (p_2 - p_1) \\cdot n\\f]\n"
        "Jacobian (eq 28):\n"
        "\\f[J = \\begin{bmatrix} -n^T & (-(r_1 + u) \\times n)^T & n^T & (r_2 \\times n)^T \\end{bmatrix}\\f]\n"
        "Used terms (here and below, everything in world space):\\n\n"
        "n = constraint axis (normalized).\\n\n"
        "p1, p2 = constraint points.\\n\n"
        "r1 = p1 - x1.\\n\n"
        "r2 = p2 - x2.\\n\n"
        "u = x2 + r2 - x1 - r1 = p2 - p1.\\n\n"
        "x1, x2 = center of mass for the bodies.\\n\n"
        "v = [v1, w1, v2, w2].\\n\n"
        "v1, v2 = linear velocity of body 1 and 2.\\n\n"
        "w1, w2 = angular velocity of body 1 and 2.\\n\n"
        "M = mass matrix, a diagonal matrix of the mass and inertia with diagonal [m1, I1, m2, I2].\\n\n"
        "\\f$K^{-1} = \\left( J M^{-1} J^T \\right)^{-1}\\f$ = effective mass.\\n\n"
        "b = velocity bias.\\n\n"
        "\\f$\\beta\\f$ = baumgarte constant.");
    axisConstraintPartCls
        // .def(nb::init<MotionProperties *, float , MotionProperties *, float , Vec3Arg , float>(), "inv_mass1"_a, "inv_i1"_a, "r1_plus_u"_a, "inv_mass2"_a, "inv_i2"_a, "r2"_a, "world_space_axis"_a, "bias"_a = 0.0f,
            // "Templated form of CalculateConstraintProperties with the motion types baked in")
        .def("calculate_constraint_properties", &AxisConstraintPart::CalculateConstraintProperties, "body1"_a, "r1_plus_u"_a, "body2"_a, "r2"_a, "world_space_axis"_a, "bias"_a = 0.0f,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    r1_plus_u (Vec3): See equations above (r1 + u).\n"
            "    r2 (Vec3): See equations above (r2).\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized, pointing from body 1 to 2).\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.")
        .def("calculate_constraint_properties_with_mass_override", &AxisConstraintPart::CalculateConstraintPropertiesWithMassOverride, "body1"_a, "inv_mass1"_a, "inv_inertia_scale1"_a, "r1_plus_u"_a, "body2"_a, "inv_mass2"_a, "inv_inertia_scale2"_a, "r2"_a, "world_space_axis"_a, "bias"_a = 0.0f,
            "Calculate properties used during the functions below, version that supports mass scaling.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    inv_mass1 (float): The inverse mass of body 1 (only used when body 1 is dynamic).\n"
            "    inv_mass2 (float): The inverse mass of body 2 (only used when body 2 is dynamic).\n"
            "    inv_inertia_scale1 (float): Scale factor for the inverse inertia of body 1.\n"
            "    inv_inertia_scale2 (float): Scale factor for the inverse inertia of body 2.\n"
            "    r1_plus_u (Vec3): See equations above (r1 + u).\n"
            "    r2 (Vec3): See equations above (r2).\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized, pointing from body 1 to 2).\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.")
        .def("calculate_constraint_properties_with_frequency_and_damping", &AxisConstraintPart::CalculateConstraintPropertiesWithFrequencyAndDamping, "delta_time"_a, "body1"_a, "r1_plus_u"_a, "body2"_a, "r2"_a, "world_space_axis"_a, "bias"_a, "c"_a, "frequency"_a, "damping"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    delta_time (float): Time step.\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    r1_plus_u (Vec3): See equations above (r1 + u).\n"
            "    r2 (Vec3): See equations above (r2).\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized, pointing from body 1 to 2).\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    frequency (float): Oscillation frequency (Hz).\n"
            "    damping (float): Damping factor (0 = no damping, 1 = critical damping).")
        .def("calculate_constraint_properties_with_stiffness_and_damping", &AxisConstraintPart::CalculateConstraintPropertiesWithStiffnessAndDamping, "delta_time"_a, "body1"_a, "r1_plus_u"_a, "body2"_a, "r2"_a, "world_space_axis"_a, "bias"_a, "c"_a, "stiffness"_a, "damping"_a,
            "Calculate properties used during the functions below.\n"
            "Args:\n"
            "    delta_time (float): Time step.\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    r1_plus_u (Vec3): See equations above (r1 + u).\n"
            "    r2 (Vec3): See equations above (r2).\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized, pointing from body 1 to 2).\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    stiffness (float): Spring stiffness k.\n"
            "    damping (float): Spring damping coefficient c.")
        .def("calculate_constraint_properties_with_settings", &AxisConstraintPart::CalculateConstraintPropertiesWithSettings, "delta_time"_a, "body1"_a, "r1_plus_u"_a, "body2"_a, "r2"_a, "world_space_axis"_a, "bias"_a, "c"_a, "spring_settings"_a,
            "Selects one of the above functions based on the spring settings")
        .def("deactivate", &AxisConstraintPart::Deactivate,
            "Deactivate this constraint")
        .def("is_active", &AxisConstraintPart::IsActive,
            "Check if constraint is active")
        // .def("templated_warm_start", &AxisConstraintPart::TemplatedWarmStart, "motion_properties1"_a, "inv_mass1"_a, "motion_properties2"_a, "inv_mass2"_a, "world_space_axis"_a, "warm_start_impulse_ratio"_a,
            // "Templated form of WarmStart with the motion types baked in")
        .def("warm_start", &AxisConstraintPart::WarmStart, "body1"_a, "body2"_a, "world_space_axis"_a, "warm_start_impulse_ratio"_a,
            "Must be called from the WarmStartVelocityConstraint call to apply the previous frame's impulses.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized).\n"
            "    warm_start_impulse_ratio (float): Ratio of new step to old time step (dt_new / dt_old) for scaling the lagrange multiplier of the previous frame.")
        // .def("templated_solve_velocity_constraint_get_total_lambda", &AxisConstraintPart::TemplatedSolveVelocityConstraintGetTotalLambda, "motion_properties1"_a, "motion_properties2"_a, "world_space_axis"_a,
            // "Templated form of SolveVelocityConstraint with the motion types baked in, part 1: get the total lambda")
        // .def("templated_solve_velocity_constraint_apply_lambda", &AxisConstraintPart::TemplatedSolveVelocityConstraintApplyLambda, "motion_properties1"_a, "inv_mass1"_a, "motion_properties2"_a, "inv_mass2"_a, "world_space_axis"_a, "total_lambda"_a,
            // "Templated form of SolveVelocityConstraint with the motion types baked in, part 2: apply new lambda")
        // .def("templated_solve_velocity_constraint", &AxisConstraintPart::TemplatedSolveVelocityConstraint, "motion_properties1"_a, "inv_mass1"_a, "motion_properties2"_a, "inv_mass2"_a, "world_space_axis"_a, "min_lambda"_a, "max_lambda"_a,
            // "Templated form of SolveVelocityConstraint with the motion types baked in")
        .def("solve_velocity_constraint", &AxisConstraintPart::SolveVelocityConstraint, "body1"_a, "body2"_a, "world_space_axis"_a, "min_lambda"_a, "max_lambda"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized).\n"
            "    min_lambda (float): Minimum value of constraint impulse to apply (N s).\n"
            "    max_lambda (float): Maximum value of constraint impulse to apply (N s).")
        .def("solve_velocity_constraint_with_mass_override", &AxisConstraintPart::SolveVelocityConstraintWithMassOverride, "body1"_a, "inv_mass1"_a, "body2"_a, "inv_mass2"_a, "world_space_axis"_a, "min_lambda"_a, "max_lambda"_a,
            "Iteratively update the velocity constraint. Makes sure d/dt C(...) = 0, where C is the constraint equation.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    inv_mass1 (float): The inverse mass of body 1 (only used when body 1 is dynamic).\n"
            "    inv_mass2 (float): The inverse mass of body 2 (only used when body 2 is dynamic).\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized).\n"
            "    min_lambda (float): Minimum value of constraint impulse to apply (N s).\n"
            "    max_lambda (float): Maximum value of constraint impulse to apply (N s).")
        .def("solve_position_constraint", &AxisConstraintPart::SolvePositionConstraint, "body1"_a, "body2"_a, "world_space_axis"_a, "c"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized).\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("solve_position_constraint_with_mass_override", &AxisConstraintPart::SolvePositionConstraintWithMassOverride, "body1"_a, "inv_mass1"_a, "body2"_a, "inv_mass2"_a, "world_space_axis"_a, "c"_a, "baumgarte"_a,
            "Iteratively update the position constraint. Makes sure C(...) = 0.\n"
            "Args:\n"
            "    body1 (Body): The first body that this constraint is attached to.\n"
            "    body2 (Body): The second body that this constraint is attached to.\n"
            "    inv_mass1 (float): The inverse mass of body 1 (only used when body 1 is dynamic).\n"
            "    inv_mass2 (float): The inverse mass of body 2 (only used when body 2 is dynamic).\n"
            "    world_space_axis (Vec3): Axis along which the constraint acts (normalized).\n"
            "    c (float): Value of the constraint equation (C).\n"
            "    baumgarte (float): Baumgarte constant (fraction of the error to correct).")
        .def("set_total_lambda", &AxisConstraintPart::SetTotalLambda, "lambda_"_a,
            "Override total lagrange multiplier, can be used to set the initial value for warm starting")
        .def("get_total_lambda", &AxisConstraintPart::GetTotalLambda,
            "Return lagrange multiplier")
        .def("save_state", &AxisConstraintPart::SaveState, "stream"_a,
            "Save state of this constraint part")
        .def("restore_state", &AxisConstraintPart::RestoreState, "stream"_a,
            "Restore state of this constraint part");
}