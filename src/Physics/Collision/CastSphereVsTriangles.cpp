#include "Common.h"
#include <Jolt/Physics/Collision/CastSphereVsTriangles.h>

void BindCastSphereVsTriangles(nb::module_ &m) {
    nb::class_<CastSphereVsTriangles> castSphereVsTrianglesCls(m, "CastSphereVsTriangles",
        "Collision detection helper that casts a sphere vs one or more triangles");
    castSphereVsTrianglesCls
        .def(nb::init<const ShapeCast &, const ShapeCastSettings &, Vec3Arg, Mat44Arg, const SubShapeIDCreator &, CastShapeCollector &>(), "shape_cast"_a, "shape_cast_settings"_a, "scale"_a, "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "collector"_a,
            "Constructor.\n"
            "Args:\n"
            "    shape_cast (ShapeCast): The sphere to cast against the triangles and its start and direction.\n"
            "    shape_cast_settings (ShapeCastSettings): Settings for performing the cast.\n"
            "    scale (Vec3): Local space scale for the shape to cast against (scales relative to its center of mass).\n"
            "    center_of_mass_transform2 (Mat44): Is the center of mass transform of shape 2 (excluding scale), this is used to provide a transform to the shape cast result so that local quantities can be transformed into world space.\n"
            "    sub_shape_id_creator1 (SubShapeIDCreator): Class that tracks the current sub shape ID for the casting shape.\n"
            "    collector (CastShapeCollector): The collector that receives the results.")
        .def("cast", &CastSphereVsTriangles::Cast, "v0"_a, "v1"_a, "v2"_a, "active_edges"_a, "sub_shape_id2"_a,
            "Cast sphere with a single triangle.\n"
            "Args:\n"
            "    v0 (Vec3): , inV1 , inV2: CCW triangle vertices.\n"
            "    active_edges (int): bit 0 = edge v0..v1 is active, bit 1 = edge v1..v2 is active, bit 2 = edge v2..v0 is active\n"
            "An active edge is an edge that is not connected to another triangle in such a way that it is impossible to collide with the edge.\n"
            "    sub_shape_id2 (SubShapeID): The sub shape ID for the triangle.");
}