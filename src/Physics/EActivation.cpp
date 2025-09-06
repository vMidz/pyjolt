#include "Common.h"
#include <Jolt/Physics/EActivation.h>

void BindEActivation(nb::module_ &m) {
    nb::enum_<EActivation>(m, "EActivation",
        "Enum used by AddBody to determine if the body needs to be initially active")
        .value("ACTIVATE", EActivation::Activate,
            "Activate the body, making it part of the simulation")
        .value("DONT_ACTIVATE", EActivation::DontActivate,
            "Leave activation state as it is (will not deactivate an active body)");
}