#include "Common.h"
#include <Jolt/Physics/Collision/Shape/SubShapeIDPair.h>
#include <nanobind/operators.h>

void BindSubShapeIDPair(nb::module_ &m) {
    nb::class_<SubShapeIDPair> subShapeIDPairCls(m, "SubShapeIDPair",
        "A pair of bodies and their sub shape ID's. Can be used as a key in a map to find a contact point.");
    subShapeIDPairCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const BodyID &, const SubShapeID &, const BodyID &, const SubShapeID &>(), "body1_id"_a, "sub_shape_id1"_a, "body2_id"_a, "sub_shape_id2"_a)
        .def("assign", &SubShapeIDPair::operator=)
        .def(nb::init<const SubShapeIDPair &>())
        .def(nb::self == nb::self, "rhs"_a, "Equality operator")
        .def(nb::self < nb::self, "rhs"_a, "Less than operator, used to consistently order contact points for a deterministic simulation")
        .def("get_body1_id", &SubShapeIDPair::GetBody1ID)
        .def("get_sub_shape_id1", &SubShapeIDPair::GetSubShapeID1)
        .def("get_body2_id", &SubShapeIDPair::GetBody2ID)
        .def("get_sub_shape_id2", &SubShapeIDPair::GetSubShapeID2)
        .def("get_hash", &SubShapeIDPair::GetHash)
        .def("__hash__", [](const SubShapeIDPair &self) {
            return self.GetHash();
        });
}