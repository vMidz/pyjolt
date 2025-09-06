#include "Common.h"
#include <Jolt/Geometry/Sphere.h>

void BindSphere(nb::module_ &m) {
    nb::class_<Sphere>(m, "Sphere")
        .def(nb::init<>(), "Default Constructor")
        .def(nb::init<Float3, float>(), "center"_a, "radius"_a)
        .def(nb::init<Vec3Arg, float>(), "center"_a, "radius"_a)
        .def("get_support", &Sphere::GetSupport, "direction"_a, "Calculate the support vector for this convex shape.")
        .def("get_center", &Sphere::GetCenter, "Properties")
        .def("get_radius", &Sphere::GetRadius)
        .def("overlaps", nb::overload_cast<const Sphere &>(&Sphere::Overlaps, nb::const_), "b"_a, "Test if two spheres overlap")
        .def("overlaps", nb::overload_cast<const AABox &>(&Sphere::Overlaps, nb::const_), "other"_a, "Check if this sphere overlaps with a box")
        .def("encapsulate_point", &Sphere::EncapsulatePoint, "point"_a, "Create the minimal sphere that encapsulates this sphere and inPoint");
}