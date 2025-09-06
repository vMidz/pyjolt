#include "Common.h"
#include <Jolt/Geometry/Ellipse.h>

void BindEllipse(nb::module_ &m) {
    nb::class_<Ellipse>(m, "Ellipse", "Ellipse centered around the origin")
        .def(nb::init<float, float>(), "a"_a, "b"_a)
        .def("is_inside", &Ellipse::IsInside, "point"_a, "Check if inPoint is inside the ellipse")
        .def("get_closest_point", &Ellipse::GetClosestPoint, "point"_a,
            "Get the closest point on the ellipse to inPoint\n"
            "Assumes inPoint is outside the ellipse\n"
            "References: \n"
            "    Rotation Joint Limits in Quaternion Space by Gino van den Bergen, section 10.1 in Game Engine Gems 3.")
        .def("get_normal", &Ellipse::GetNormal, "point"_a, "Get normal at point inPoint (non-normalized vector)");
}