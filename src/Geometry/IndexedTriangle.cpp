#include "Common.h"
#include <Jolt/Geometry/IndexedTriangle.h>
#include <nanobind/operators.h>

void BindIndexedTriangle(nb::module_ &m) {
    nb::class_<IndexedTriangleNoMaterial>(m, "IndexedTriangleNoMaterial", "Triangle with 32-bit indices")
        .def(nb::init<>(), "Constructor")
        .def(nb::init<uint32, uint32, uint32>(), "l_1"_a, "l_2"_a, "l_3"_a)

        .def(nb::self == nb::self, "rhs"_a, "Check if two triangles are identical")
        .def("is_equivalent", &IndexedTriangleNoMaterial::IsEquivalent,
            "rhs"_a, "Check if two triangles are equivalent (using the same vertices)")
        .def("is_opposite", &IndexedTriangleNoMaterial::IsOpposite,
            "rhs"_a, "Check if two triangles are opposite (using the same vertices but in opposing order)")
        .def("is_degenerate", &IndexedTriangleNoMaterial::IsDegenerate, "vertices"_a, "Check if triangle is degenerate")
        .def("rotate", &IndexedTriangleNoMaterial::Rotate,
            "Rotate the vertices so that the second vertex becomes first etc. This does not change the represented triangle.")
        .def("get_centroid", &IndexedTriangleNoMaterial::GetCentroid, "vertices"_a, "Get center of triangle")
        .def_prop_rw("idx", [](IndexedTriangleNoMaterial &self, int index) {
            if (index >= 3)
                throw nb::index_error("Index out of range");

            return self.mIdx[index];
            },
            [](IndexedTriangleNoMaterial &self, int index, uint32 value) {
            if (index >= 3)
                throw nb::index_error("Index out of range");

            self.mIdx[index] = value;
            }, "index"_a, "value"_a);

    nb::class_<IndexedTriangle, IndexedTriangleNoMaterial>(m, "IndexedTriangle")
        .def(nb::init<uint32, uint32, uint32>(), "l_1"_a, "l_2"_a, "l_3"_a)

        .def(nb::self == nb::self, "rhs"_a, "Check if two triangles are identical")
        .def("get_lowest_index_first", &IndexedTriangle::GetLowestIndexFirst,
            "Rotate the vertices so that the lowest vertex becomes the first. This does not change the represented triangle.")
        .def_rw("material_index", &IndexedTriangle::mMaterialIndex)
        .def_rw("user_data", &IndexedTriangle::mUserData,
            "User data that can be used for anything by the application, e.g. for tracking the original index of the triangle");
}