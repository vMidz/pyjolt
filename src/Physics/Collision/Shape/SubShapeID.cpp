#include "Common.h"
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <nanobind/operators.h>

void BindSubShapeID(nb::module_ &m) {
    nb::class_<SubShapeID> subShapeIDCls(m, "SubShapeID",
        "@brief A sub shape id contains a path to an element (usually a triangle or other primitive type) of a compound shape\n"
        "Each sub shape knows how many bits it needs to encode its ID, so knows how many bits to take from the sub shape ID.\n"
        "For example:\n"
        "* We have a CompoundShape A with 5 child shapes (identify sub shape using 3 bits AAA)\n"
        "* One of its child shapes is CompoundShape B which has 3 child shapes (identify sub shape using 2 bits BB)\n"
        "* One of its child shapes is MeshShape C which contains enough triangles to need 7 bits to identify a triangle (identify sub shape using 7 bits CCCCCCC, note that MeshShape is block based and sorts triangles spatially, you can't assume that the first triangle will have bit pattern 0000000).\n"
        "The bit pattern of the sub shape ID to identify a triangle in MeshShape C will then be CCCCCCCBBAAA.\n"
        "A sub shape ID will become invalid when the structure of the shape changes. For example, if a child shape is removed from a compound shape, the sub shape ID will no longer be valid.\n"
        "This can be a problem when caching sub shape IDs from one frame to the next. See comments at ContactListener::OnContactPersisted / OnContactRemoved.");
    subShapeIDCls
        .def(nb::init<>(),
            "Constructor")
        .def("pop_id", &SubShapeID::PopID, "bits"_a, "remainder"_a,
            "Get the next id in the chain of ids (pops parents before children)")
        .def("get_value", &SubShapeID::GetValue,
            "Get the value of the path to the sub shape ID")
        .def("set_value", &SubShapeID::SetValue, "value"_a,
            "Set the value of the sub shape ID (use with care!)")
        .def("is_empty", &SubShapeID::IsEmpty,
            "Check if there is any bits of subshape ID left.\n"
            "Note that this is not a 100% guarantee as the subshape ID could consist of all 1 bits. Use for asserts only.")
        .def(nb::self == nb::self, "rhs"_a,
            "Check equal")
        .def(nb::self != nb::self, "rhs"_a,
            "Check not-equal")
        .def_ro_static("MAX_BITS", &SubShapeID::MaxBits,
            "How many bits we can store in this ID");

    nb::class_<SubShapeIDCreator> subShapeIDCreatorCls(m, "SubShapeIDCreator",
        "A sub shape id creator can be used to create a new sub shape id by recursing through the shape\n"
        "hierarchy and pushing new ID's onto the chain");
    subShapeIDCreatorCls
        .def("push_id", &SubShapeIDCreator::PushID, "value"_a, "bits"_a,
            "Add a new id to the chain of id's and return it")
        .def("get_id", &SubShapeIDCreator::GetID)
        .def("get_num_bits_written", &SubShapeIDCreator::GetNumBitsWritten,
            "Get the number of bits that have been written to the sub shape ID so far");
}
