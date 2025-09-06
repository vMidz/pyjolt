#include "Common.h"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

void BindBoxShape(nb::module_ &m) {
    nb::class_<BoxShapeSettings, ConvexShapeSettings> boxShapeSettingsCls(m, "BoxShapeSettings",
        "Class that constructs a BoxShape",
        nb::is_final());
    boxShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<Vec3Arg, float, const PhysicsMaterial *>(), "half_extent"_a, "convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr,
            "Create a box with half edge length inHalfExtent and convex radius inConvexRadius.\n"
            "(internally the convex radius will be subtracted from the half extent so the total box will not grow with the convex radius).")
        .def("create", &BoxShapeSettings::Create)
        .def_rw("half_extent", &BoxShapeSettings::mHalfExtent,
            "Half the size of the box (including convex radius)")
        .def_rw("convex_radius", &BoxShapeSettings::mConvexRadius);

    nb::class_<BoxShape, ConvexShape> boxShapeCls(m, "BoxShape",
        "A box, centered around the origin",
        nb::is_final());
    boxShapeCls
        .def(nb::init<>(),
        "Constructor")
        .def(nb::init<const BoxShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<Vec3Arg, float, const PhysicsMaterial *>(), "half_extent"_a, "convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr,
            "Create a box with half edge length inHalfExtent and convex radius inConvexRadius.\n"
            "(internally the convex radius will be subtracted from the half extent so the total box will not grow with the convex radius).")
        .def("get_half_extent", &BoxShape::GetHalfExtent,
            "Get half extent of box")
        .def("get_local_bounds", &BoxShape::GetLocalBounds)
        .def("get_inner_radius", &BoxShape::GetInnerRadius)
        .def("get_mass_properties", &BoxShape::GetMassProperties)
        .def("get_surface_normal", &BoxShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &BoxShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &BoxShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("draw", &BoxShape::Draw,
            "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&BoxShape::CastRay, nb::const_),
            "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &,
            const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&BoxShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &BoxShape::CollidePoint,
            "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &BoxShape::CollideSoftBodyVertices,
            "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &BoxShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &BoxShape::GetTrianglesNext,
            // "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr) // TODO:
        .def("save_binary_state", &BoxShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &BoxShape::GetStats)
        .def("get_volume", &BoxShape::GetVolume)
        .def("get_convex_radius", &BoxShape::GetConvexRadius,
            "Get the convex radius of this box")
        .def_static("register", &BoxShape::sRegister);
}