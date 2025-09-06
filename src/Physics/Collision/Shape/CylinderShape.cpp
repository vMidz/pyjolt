#include "Common.h"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

void BindCylinderShape(nb::module_ &m) {
    nb::class_<CylinderShapeSettings, ConvexShapeSettings> cylinderShapeSettingsCls(m, "CylinderShapeSettings",
        "Class that constructs a CylinderShape",
        nb::is_final());
    cylinderShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<float, float, float, const PhysicsMaterial *>(), "half_height"_a, "radius"_a, "convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr,
            "Create a shape centered around the origin with one top at (0, -inHalfHeight, 0) and the other at (0, inHalfHeight, 0) and radius inRadius.\n"
            "(internally the convex radius will be subtracted from the cylinder the total cylinder will not grow with the convex radius, but the edges of the cylinder will be rounded a bit).")
        .def("create", &CylinderShapeSettings::Create)
        .def_rw("half_height", &CylinderShapeSettings::mHalfHeight)
        .def_rw("radius", &CylinderShapeSettings::mRadius)
        .def_rw("convex_radius", &CylinderShapeSettings::mConvexRadius);

    nb::class_<CylinderShape, ConvexShape> cylinderShapeCls(m, "CylinderShape",
        "A cylinder",
        nb::is_final());
    cylinderShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const CylinderShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<float, float, float, const PhysicsMaterial *>(), "half_height"_a, "radius"_a, "convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr,
            "Create a shape centered around the origin with one top at (0, -inHalfHeight, 0) and the other at (0, inHalfHeight, 0) and radius inRadius.\n"
            "(internally the convex radius will be subtracted from the cylinder the total cylinder will not grow with the convex radius, but the edges of the cylinder will be rounded a bit).")
        .def("get_half_height", &CylinderShape::GetHalfHeight,
            "Get half height of cylinder")
        .def("get_radius", &CylinderShape::GetRadius,
            "Get radius of cylinder")
        .def("get_local_bounds", &CylinderShape::GetLocalBounds)
        .def("get_inner_radius", &CylinderShape::GetInnerRadius)
        .def("get_mass_properties", &CylinderShape::GetMassProperties)
        .def("get_surface_normal", &CylinderShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &CylinderShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &CylinderShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("draw", &CylinderShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&CylinderShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("collide_point", &CylinderShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &CylinderShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &CylinderShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &CylinderShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &CylinderShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &CylinderShape::GetStats)
        .def("get_volume", &CylinderShape::GetVolume)
        .def("get_convex_radius", &CylinderShape::GetConvexRadius,
            "Get the convex radius of this cylinder")
        .def("is_valid_scale", &CylinderShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &CylinderShape::MakeScaleValid, "scale"_a)
        .def_static("register", &CylinderShape::sRegister)
        .def("__repr__", [](const CylinderShape &self) {
            return nb::str("CylinderShape(half_height={}, radius={}, convex_radius={})")
                .format(self.GetHalfHeight(), self.GetRadius(), self.GetConvexRadius());
        });
}