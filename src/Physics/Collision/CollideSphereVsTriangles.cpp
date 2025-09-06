#include "Common.h"
#include <Jolt/Physics/Collision/CollideSphereVsTriangles.h>
#include <Jolt/Physics/Collision/CollideShape.h>

void BindCollideSphereVsTriangles(nb::module_ &m) {
    nb::class_<CollideSphereVsTriangles> collideSphereVsTrianglesCls(m, "CollideSphereVsTriangles",
        "Collision detection helper that collides a sphere vs one or more triangles");
    collideSphereVsTrianglesCls
        .def(nb::init<const SphereShape *, Vec3Arg, Vec3Arg, Mat44Arg, Mat44Arg, const SubShapeID &, const CollideShapeSettings &, CollideShapeCollector &>(), "shape1"_a, "scale1"_a, "scale2"_a, "center_of_mass_transform1"_a, "center_of_mass_transform2"_a, "sub_shape_id1"_a, "collide_shape_settings"_a, "collector"_a,
            "Constructor.\n"
            "Args:\n"
            "    shape1 (SphereShape*): The sphere to collide against triangles.\n"
            "    scale1 (Vec3): Local space scale for the sphere (scales relative to its center of mass).\n"
            "    scale2 (Vec3): Local space scale for the triangles.\n"
            "    center_of_mass_transform1 (Mat44): Transform that takes the center of mass of 1 into world space.\n"
            "    center_of_mass_transform2 (Mat44): Transform that takes the center of mass of 2 into world space.\n"
            "    sub_shape_id1 (SubShapeID): Sub shape ID of the convex object.\n"
            "    collide_shape_settings (CollideShapeSettings): Settings for the collide shape query.\n"
            "    collector (CollideShapeCollector): The collector that will receive the results.")
        .def("collide", &CollideSphereVsTriangles::Collide, "v0"_a, "v1"_a, "v2"_a, "active_edges"_a, "sub_shape_id2"_a,
            "Collide sphere with a single triangle.\n"
            "Args:\n"
            "    v0 (Vec3): , inV1 , inV2: CCW triangle vertices.\n"
            "    active_edges (int): bit 0 = edge v0..v1 is active, bit 1 = edge v1..v2 is active, bit 2 = edge v2..v0 is active\n"
            "An active edge is an edge that is not connected to another triangle in such a way that it is impossible to collide with the edge.\n"
            "    sub_shape_id2 (SubShapeID): The sub shape ID for the triangle.");
}