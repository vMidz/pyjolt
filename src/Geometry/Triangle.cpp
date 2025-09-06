#include "Common.h"
#include <Jolt/Geometry/Triangle.h>

void BindTriangle(nb::module_ &m) {
    nb::class_<Triangle>(m, "Triangle", "A simple triangle and its material")
        .def(nb::init<>(), "Constructor")
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg, uint32, uint32>(),
             "v1"_a, "v2"_a, "v3"_a, "material_index"_a = 0, "user_data"_a = 0)

        .def(nb::init<const Float3 &, const Float3 &, const Float3 &, uint32, uint32>(),
             "v1"_a, "v2"_a, "v3"_a, "material_index"_a = 0, "user_data"_a = 0)

        .def("get_centroid", &Triangle::GetCentroid, "Get center of triangle")

        .def_prop_rw("vertices", [](const Triangle &self, int i) {
            if (i < 0 || i >= 3)
                throw std::out_of_range("Vertex index out of range");
            return self.mV[i];
        }, [](Triangle &self, int i, const Float3 &v) {
            if (i < 0 || i >= 3)
                throw std::out_of_range("Vertex index out of range");
            self.mV[i] = v;})

        .def_rw("m_material_index", &Triangle::mMaterialIndex,
            "Follows mV[3] so that we can read mV as 4 vectors")
        .def_rw("m_user_data", &Triangle::mUserData,
            "User data that can be used for anything by the application, e.g. for tracking the original index of the triangle")
        .def("get_vertices", [](Triangle &self) {
            nb::list r;
            for (size_t i = 0; i < 3; ++i)
                r.append(self.mV);
            return r;
        });
}