#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintPart/SpringPart.h>

void BindSpringPart(nb::module_ &m) {
    nb::class_<SpringPart> springPartCls(m, "SpringPart",
        "Class used in other constraint parts to calculate the required bias factor in the lagrange multiplier for creating springs");
    springPartCls
        .def("calculate_spring_properties_with_bias", &SpringPart::CalculateSpringPropertiesWithBias, "bias"_a,
            "Turn off the spring and set a bias only.\n"
            "Args:\n"
            "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.")
        .def("calculate_spring_properties_with_frequency_and_damping", [](SpringPart &self, float inDeltaTime, float inInvEffectiveMass, float inBias, float inC, float inFrequency, float inDamping) {
            float outEffectiveMass;
            self.CalculateSpringPropertiesWithFrequencyAndDamping(inDeltaTime, inInvEffectiveMass, inBias, inC, inFrequency, inDamping, outEffectiveMass);
            return outEffectiveMass;
            }, "delta_time"_a, "inv_effective_mass"_a, "bias"_a, "c"_a, "frequency"_a, "damping"_a,
               "Calculate spring properties based on frequency and damping ratio.\n"
               "Args:\n"
               "    delta_time (float): Time step.\n"
               "    inv_effective_mass (float): Inverse effective mass K.\n"
               "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.\n"
               "    c (float): Value of the constraint equation (C). Set to zero if you don't want to drive the constraint to zero with a spring.\n"
               "    frequency (float): Oscillation frequency (Hz). Set to zero if you don't want to drive the constraint to zero with a spring.\n"
               "    damping (float): Damping factor (0 = no damping, 1 = critical damping). Set to zero if you don't want to drive the constraint to zero with a spring.\n"
               "    effective_mass (float): On return, this contains the new effective mass K^-1.")
        .def("calculate_spring_properties_with_stiffness_and_damping",[](SpringPart &self, float inDeltaTime, float inInvEffectiveMass, float inBias, float inC, float inStiffness, float inDamping) {
            float outEffectiveMass;
            self.CalculateSpringPropertiesWithStiffnessAndDamping(inDeltaTime, inInvEffectiveMass, inBias, inC, inStiffness, inDamping, outEffectiveMass);
            return outEffectiveMass;
            }, "delta_time"_a, "inv_effective_mass"_a, "bias"_a, "c"_a, "stiffness"_a, "damping"_a,
               "Calculate spring properties with spring Stiffness (k) and damping (c), this is based on the spring equation: F = -k * x - c * v.\n"
               "Args:\n"
               "    delta_time (float): Time step.\n"
               "    inv_effective_mass (float): Inverse effective mass K.\n"
               "    bias (float): Bias term (b) for the constraint impulse: lambda = J v + b.\n"
               "    c (float): Value of the constraint equation (C). Set to zero if you don't want to drive the constraint to zero with a spring.\n"
               "    stiffness (float): Spring stiffness k. Set to zero if you don't want to drive the constraint to zero with a spring.\n"
               "    damping (float): Spring damping coefficient c. Set to zero if you don't want to drive the constraint to zero with a spring.\n"
               "    effective_mass (float): On return, this contains the new effective mass K^-1.")
        .def("is_active", &SpringPart::IsActive,
            "Returns if this spring is active")
        .def("get_bias", &SpringPart::GetBias, "total_lambda"_a,
            "Get total bias b, including supplied bias and bias for spring: lambda = J v + b");
}