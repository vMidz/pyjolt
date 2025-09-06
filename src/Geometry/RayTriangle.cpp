#include "Common.h"
#include <Jolt/Geometry/RayTriangle.h>

void BindRayTriangle(nb::module_ &m) {
    m.def("ray_triangle", &RayTriangle,
          "origin"_a,
          "direction"_a,
          "v_0"_a,
          "v_1"_a,
          "v_2"_a,
          "Intersect ray with triangle, returns closest point or FLT_MAX if no hit (branch less version)\n"
          "Adapted from: http://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm");

    m.def("ray_triangle4", RayTriangle4,
          "inOrigin"_a,
          "inDirection"_a,
          "inV0X"_a,
          "inV0Y"_a,
          "inV0Z"_a,
          "inV1X"_a,
          "inV1Y"_a,
          "inV1Z"_a,
          "inV2X"_a,
          "inV2Y"_a,
          "inV2Z"_a,
          "Intersect ray with 4 triangles in SOA format, returns 4 vector of closest points or FLT_MAX if no hit (uses bit tricks to do less divisions)");
}