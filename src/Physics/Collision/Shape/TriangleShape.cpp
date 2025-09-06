#include "Common.h"
#include <Jolt/Physics/Collision/Shape/TriangleShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

void BindTriangleShape(nb::module_ &m) {
    nb::class_<TriangleShapeSettings, ConvexShapeSettings> triangleShapeSettingsCls(m, "TriangleShapeSettings",
        "Class that constructs a TriangleShape",
        nb::is_final());
    triangleShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg, float, const PhysicsMaterial *>(), "v1"_a, "v2"_a, "v3"_a, "convex_radius"_a = 0.0f, "material"_a = nullptr,
            "Create a triangle with points (inV1, inV2, inV3) (counter clockwise) and convex radius inConvexRadius.\n"
            "Note that the convex radius is currently only used for shape vs shape collision, for all other purposes the triangle is infinitely thin.")
        .def("create", &TriangleShapeSettings::Create)
        .def_rw("v1", &TriangleShapeSettings::mV1)
        .def_rw("v2", &TriangleShapeSettings::mV2)
        .def_rw("v3", &TriangleShapeSettings::mV3)
        .def_rw("convex_radius", &TriangleShapeSettings::mConvexRadius);

    nb::class_<TriangleShape, ConvexShape> triangleShapeCls(m, "TriangleShape",
        "A single triangle, not the most efficient way of creating a world filled with triangles but can be used as a query shape for example.",
        nb::is_final());
    triangleShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const TriangleShapeSettings &, Shape::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg, float, const PhysicsMaterial *>(), "v1"_a, "v2"_a, "v3"_a, "convex_radius"_a = 0.0f, "material"_a = nullptr,
            "Create a triangle with points (inV1, inV2, inV3) (counter clockwise) and convex radius inConvexRadius.\n"
            "Note that the convex radius is currently only used for shape vs shape collision, for all other purposes the triangle is infinitely thin.")
        .def("get_vertex1", &TriangleShape::GetVertex1,
            "Get the vertices of the triangle")
        .def("get_vertex2", &TriangleShape::GetVertex2)
        .def("get_vertex3", &TriangleShape::GetVertex3)
        .def("get_convex_radius", &TriangleShape::GetConvexRadius,
            "Convex radius")
        .def("get_local_bounds", &TriangleShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&TriangleShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &TriangleShape::GetInnerRadius)
        .def("get_mass_properties", &TriangleShape::GetMassProperties)
        .def("get_surface_normal", &TriangleShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &TriangleShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &TriangleShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("get_submerged_volume", [](TriangleShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,
                outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &TriangleShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&TriangleShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&TriangleShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &TriangleShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &TriangleShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &TriangleShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &TriangleShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &TriangleShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &TriangleShape::GetStats)
        .def("get_volume", &TriangleShape::GetVolume)
        .def("is_valid_scale", &TriangleShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &TriangleShape::MakeScaleValid, "scale"_a)
        .def_static("register", &TriangleShape::sRegister);
}