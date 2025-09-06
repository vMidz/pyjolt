#include "Common.h"
#include <Jolt/Geometry/RayAABox.h>

void BindRayAABox(nb::module_ &m) {
    nb::class_<RayInvDirection>(m, "RayInvDirection", "Helper structure holding the reciprocal of a ray for Ray vs AABox testing")
        .def(nb::init<>(), "Constructors")
        .def(nb::init<Vec3Arg>(), "direction"_a)
        .def("set", &RayInvDirection::Set, "direction"_a, "Set reciprocal from ray direction");

    m.def("ray_aa_box", nb::overload_cast<Vec3Arg, const RayInvDirection &, Vec3Arg, Vec3Arg>(&RayAABox),
        "origin"_a, "inv_direction"_a, "bounds_min"_a, "bounds_max"_a,
        "Intersect AABB with ray, returns minimal distance along ray or FLT_MAX if no hit\n"
        "Note: Can return negative value if ray starts in box");

    m.def("ray_aa_box", [](Vec3Arg inOrigin, const RayInvDirection &inInvDirection, Vec3Arg inBoundsMin, Vec3Arg inBoundsMax) {
        float outMin, outMax;
        RayAABox(inOrigin, inInvDirection, inBoundsMin, inBoundsMax, outMin, outMax);
        return nb::make_tuple(outMin, outMax);
    }, "origin"_a, "inv_direction"_a, "bounds_min"_a, "bounds_max"_a,
       "Intersect AABB with ray, returns minimal and maximal distance along ray or FLT_MAX, -FLT_MAX if no hit\n"
       "Note: Can return negative value for outMin if ray starts in box");

    m.def("ray_aa_box4", &RayAABox4,
        "origin"_a, "inv_direction"_a, "bounds_min_x"_a, "bounds_min_y"_a, "bounds_min_z"_a,
        "bounds_max_x"_a, "bounds_max_y"_a, "bounds_max_z"_a,
        "Intersect 4 AABBs with ray, returns minimal distance along ray or FLT_MAX if no hit\n"
        "Note: Can return negative value if ray starts in box");

    m.def("ray_aa_box_hits", nb::overload_cast<Vec3Arg, const RayInvDirection &, Vec3Arg, Vec3Arg, float>(&RayAABoxHits),
        "origin"_a, "inv_direction"_a, "bounds_min"_a, "bounds_max"_a, "closest"_a,
        "Intersect AABB with ray, returns true if there is a hit closer than inClosest");

    m.def("ray_aa_box_hits", nb::overload_cast<Vec3Arg, Vec3Arg, Vec3Arg, Vec3Arg>(&RayAABoxHits),
        "origin"_a, "direction"_a, "bounds_min"_a, "bounds_max"_a,
        "Intersect AABB with ray without hit fraction, based on separating axis test");
}