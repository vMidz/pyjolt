#include "Common.h"
#include <Jolt/Geometry/RayCapsule.h>

void BindRayCapsule(nb::module_ &m) {
    m.def("ray_capsule", &RayCapsule,
        "ray_origin"_a, "ray_direction"_a, "capsule_half_height"_a, "capsule_radius"_a,
        "Tests a ray starting at inRayOrigin and extending infinitely in inRayDirection\n"
        "against a capsule centered around the origin with its axis along the Y axis and half height specified.\n"
        "Args:\n"
        "    ray_direction (Vec3): Ray direction. Does not need to be normalized.\n"
        "    ray_origin (Vec3): Origin of the ray. If the ray starts inside the capsule, the returned fraction will be 0.\n"
        "    capsule_half_height (float): Distance from the origin to the center of the top sphere (or that of the bottom).\n"
        "    capsule_radius (float): Radius of the top/bottom sphere.\n"
        "Returns:\n"
        "    float: FLT_MAX if there is no intersection, otherwise the fraction along the ray.");
}