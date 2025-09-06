#include "Common.h"
#include <Jolt/Geometry/RayCylinder.h>

void BindRayCylinder(nb::module_ &m) {
    m.def("ray_cylinder", nb::overload_cast<Vec3Arg, Vec3Arg, float>(&RayCylinder),
        "ray_origin"_a, "ray_direction"_a, "cylinder_radius"_a,
        "Tests a ray starting at inRayOrigin and extending infinitely in inRayDirection\n"
        "against an infinite cylinder centered along the Y axis.\n"
        "Args:\n"
        "    ray_direction (Vec3): Direction of the ray. Does not need to be normalized.\n"
        "    ray_origin (Vec3): Origin of the ray. If the ray starts inside the cylinder, the returned fraction will be 0.\n"
        "    cylinder_radius (float): Radius of the infinite cylinder.\n"
        "Returns:\n"
        "    float: FLT_MAX if there is no intersection, otherwise the fraction along the ray.");

    m.def("ray_cylinder", nb::overload_cast<Vec3Arg, Vec3Arg, float, float>(&RayCylinder),
        "ray_origin"_a, "ray_direction"_a, "cylinder_half_height"_a, "cylinder_radius"_a,
        "Test a ray against a cylinder centered around the origin with its axis along the Y axis and half height specified.\n"
        "Args:\n"
        "    ray_direction (Vec3): Ray direction. Does not need to be normalized.\n"
        "    ray_origin (Vec3): Origin of the ray. If the ray starts inside the cylinder, the returned fraction will be 0.\n"
        "    cylinder_radius (float): Radius of the cylinder.\n"
        "    cylinder_half_height (float): Distance from the origin to the top (or bottom) of the cylinder.\n"
        "Returns:\n"
        "    float: FLT_MAX if there is no intersection, otherwise the fraction along the ray.");
}