#include "Common.h"
#include <Jolt/Physics/Body/BodyPair.h>
#include <nanobind/operators.h>
#include <Jolt/Physics/Body/BodyID.h>

void BindBodyPair(nb::module_ &m) {
    nb::class_<BodyPair>(m, "BodyPair",
        "Structure that holds a body pair")
        .def(nb::init<>())
        .def(nb::init<BodyID, BodyID>())
        .def(nb::self == nb::self, "rhs"_a,
            "Equals operator")
        .def(nb::self < nb::self, "rhs"_a, "Smaller than operator, used for consistently ordering body pairs")
        .def("get_hash", &BodyPair::GetHash, "Get the hash value of this object")
        .def_rw("body_a", &BodyPair::mBodyA)
        .def_rw("body_b", &BodyPair::mBodyB);
}