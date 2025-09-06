#include "Common.h"
#include <Jolt/Physics/Constraints/SpringSettings.h>
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

void BindSpringSettings(nb::module_ &m) {
    nb::enum_<ESpringMode>(m, "ESpringMode",
        "Enum used by constraints to specify how the spring is defined")
        .value("FREQUENCY_AND_DAMPING", ESpringMode::FrequencyAndDamping,
            "Frequency and damping are specified")
        .value("STIFFNESS_AND_DAMPING", ESpringMode::StiffnessAndDamping,
            "Stiffness and damping are specified");

    nb::class_<SpringSettings> springSettingsCls(m, "SpringSettings",
        "Settings for a linear or angular spring");
    springSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const SpringSettings &>())
        .def("assign", &SpringSettings::operator=)
        .def(nb::init<ESpringMode, float, float>(), "mode"_a, "frequency_or_stiffness"_a, "damping"_a)
        .def("save_binary_state", &SpringSettings::SaveBinaryState, "stream"_a,
            "Saves the contents of the spring settings in binary form to inStream.")
        .def("restore_binary_state", &SpringSettings::RestoreBinaryState, "stream"_a,
            "Restores contents from the binary stream inStream.")
        .def("has_stiffness", &SpringSettings::HasStiffness,
            "Check if the spring has a valid frequency / stiffness, if not the spring will be hard")
        .def_rw("mode", &SpringSettings::mMode,
            "Selects the way in which the spring is defined\n"
            "If the mode is StiffnessAndDamping then mFrequency becomes the stiffness (k) and mDamping becomes the damping ratio (c) in the spring equation F = -k * x - c * v. Otherwise the properties are as documented.")
        .def_rw("damping", &SpringSettings::mDamping,
            "When mSpringMode = ESpringMode::FrequencyAndDamping mDamping is the damping ratio (0 = no damping, 1 = critical damping).\n"
            "When mSpringMode = ESpringMode::StiffnessAndDamping mDamping is the damping (c) in the spring equation F = -k * x - c * v for a linear or T = -k * theta - c * w for an angular spring.\n"
            "Note that if you set mDamping = 0, you will not get an infinite oscillation. Because we integrate physics using an explicit Euler scheme, there is always energy loss.\n"
            "This is done to keep the simulation from exploding, because with a damping of 0 and even the slightest rounding error, the oscillation could become bigger and bigger until the simulation explodes.")
        .def_rw("frequency", &SpringSettings::mFrequency,
            "Valid when mSpringMode = ESpringMode::FrequencyAndDamping.\n"
            "If mFrequency > 0 the constraint will be soft and mFrequency specifies the oscillation frequency in Hz.\n"
            "If mFrequency <= 0, mDamping is ignored and the constraint will have hard limits (as hard as the time step / the number of velocity / position solver steps allows).")
        .def_rw("stiffness", &SpringSettings::mStiffness,
            "Valid when mSpringMode = ESpringMode::StiffnessAndDamping.\n"
            "If mStiffness > 0 the constraint will be soft and mStiffness specifies the stiffness (k) in the spring equation F = -k * x - c * v for a linear or T = -k * theta - c * w for an angular spring.\n"
            "If mStiffness <= 0, mDamping is ignored and the constraint will have hard limits (as hard as the time step / the number of velocity / position solver steps allows).\n"
            "Note that stiffness values are large numbers. To calculate a ballpark value for the needed stiffness you can use:\n"
            "force = stiffness * delta_spring_length = mass * gravity <=> stiffness = mass * gravity / delta_spring_length.\n"
            "So if your object weighs 1500 kg and the spring compresses by 2 meters, you need a stiffness in the order of 1500 * 9.81 / 2 ~ 7500 N/m.");
}