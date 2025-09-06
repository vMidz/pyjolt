#include "Common.h"
#include <Jolt/Physics/Collision/ActiveEdges.h>

void BindActiveEdges(nb::module_ &m) {
    using namespace ActiveEdges;
    m.def("is_edge_active", &IsEdgeActive, "normal1"_a, "normal2"_a, "edge_direction"_a, "cos_threshold_angle"_a,
        "Helper function to check if an edge is active or not.\n"
        "Args:\n"
        "    normal1 (Vec3): Triangle normal of triangle on the left side of the edge (when looking along the edge from the top).\n"
        "    normal2 (Vec3): Triangle normal of triangle on the right side of the edge.\n"
        "    edge_direction (Vec3): Vector that points along the edge.\n"
        "    cos_threshold_angle (float): Cosine of the threshold angle (if the angle between the two triangles is bigger than this, the edge is active, note that a concave edge is always inactive).");

    m.def("fix_normal", &FixNormal, "v0"_a, "v1"_a, "v2"_a, "triangle_normal"_a, "active_edges"_a, "point"_a, "normal"_a, "movement_direction"_a,
        "Replace normal by triangle normal if a hit is hitting an inactive edge.\n"
        "Args:\n"
        "    v0 (Vec3): , inV1 , inV2 form the triangle.\n"
        "    triangle_normal (Vec3): is the normal of the provided triangle (does not need to be normalized).\n"
        "    active_edges (int): bit 0 = edge v0..v1 is active, bit 1 = edge v1..v2 is active, bit 2 = edge v2..v0 is active.\n"
        "    point (Vec3): Collision point on the triangle.\n"
        "    normal (Vec3): Collision normal on the triangle (does not need to be normalized).\n"
        "    movement_direction (Vec3): Can be zero. This gives an indication of in which direction the motion is to determine if when we hit an inactive edge/triangle we should return the triangle normal.\n"
        "Returns:\n"
        "    Vec3: Returns inNormal if an active edge was hit, otherwise returns inTriangleNormal.");
}