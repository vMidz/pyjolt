#include "Common.h"
#include <Jolt/Geometry/RaySphere.h>

void BindRaySphere(nb::module_ &m) {
    m.def("ray_sphere", nb::overload_cast<Vec3Arg, Vec3Arg, Vec3Arg, float>(&RaySphere),
        "ray_origin"_a, "ray_direction"_a, "sphere_center"_a, "sphere_radius"_a,
        "Tests a ray starting at inRayOrigin and extending infinitely in inRayDirection against a sphere,.\n"
        "Args:\n"
        "    ray_origin (Vec3): Ray origin. If the ray starts inside the sphere, the returned fraction will be 0.\n"
        "    ray_direction (Vec3): Ray direction. Does not need to be normalized.\n"
        "    sphere_center (Vec3): Position of the center of the sphere.\n"
        "    sphere_radius (float): Radius of the sphere.\n"
        "Returns:\n"
        "    float: FLT_MAX if there is no intersection, otherwise the fraction along the ray.");

    m.def("ray_sphere", [](Vec3Arg inRayOrigin, Vec3Arg inRayDirection, Vec3Arg inSphereCenter, float inSphereRadius) {
        float outMinFraction, outMaxFraction;
        int intersection_count = RaySphere(inRayOrigin, inRayDirection, inSphereCenter, inSphereRadius, outMinFraction, outMaxFraction);
        return nb::make_tuple(intersection_count, outMinFraction, outMaxFraction);
    }, "ray_origin"_a, "ray_direction"_a, "sphere_center"_a, "sphere_radius"_a,
        "Tests a ray starting at inRayOrigin and extending infinitely in inRayDirection against a sphere.\n"
        "Outputs entry and exit points (outMinFraction and outMaxFraction) along the ray (which could be negative if the hit point is before the start of the ray).\n"
        "Args:\n"
        "    ray_origin (Vec3): Ray origin. If the ray starts inside the sphere, the returned fraction will be 0.\n"
        "    ray_direction (Vec3): Ray direction. Does not need to be normalized.\n"
        "    sphere_center (Vec3): Position of the center of the sphere.\n"
        "    sphere_radius (float): Radius of the sphere.\n"
        "    min_fraction (float): Returned lowest intersection fraction.\n"
        "    max_fraction (float): Returned highest intersection fraction.\n"
        "Returns:\n"
        "    int: The amount of intersections with the sphere.\n"
        "If 1 intersection is returned outMinFraction will be equal to outMaxFraction.");
}