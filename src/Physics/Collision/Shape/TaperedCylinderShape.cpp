#include "Common.h"
#include <Jolt/Physics/Collision/Shape/TaperedCylinderShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

void BindTaperedCylinderShape(nb::module_ &m) {
    nb::class_<TaperedCylinderShapeSettings, ConvexShapeSettings> taperedCylinderShapeSettingsCls(m, "TaperedCylinderShapeSettings",
        "Class that constructs a TaperedCylinderShape",
        nb::is_final());
    taperedCylinderShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<float, float, float, float, const PhysicsMaterial *>(), "half_height_of_tapered_cylinder"_a, "top_radius"_a, "bottom_radius"_a, "convex_radius"_a = cDefaultConvexRadius, "material"_a = nullptr,
            "Create a tapered cylinder centered around the origin with bottom at (0, -inHalfHeightOfTaperedCylinder, 0) with radius inBottomRadius and top at (0, inHalfHeightOfTaperedCylinder, 0) with radius inTopRadius")
        .def("create", &TaperedCylinderShapeSettings::Create)
        .def_rw("half_height", &TaperedCylinderShapeSettings::mHalfHeight)
        .def_rw("top_radius", &TaperedCylinderShapeSettings::mTopRadius)
        .def_rw("bottom_radius", &TaperedCylinderShapeSettings::mBottomRadius)
        .def_rw("convex_radius", &TaperedCylinderShapeSettings::mConvexRadius);

    nb::class_<TaperedCylinderShape, ConvexShape> taperedCylinderShapeCls(m, "TaperedCylinderShape",
        "A cylinder with different top and bottom radii",
        nb::is_final());
    taperedCylinderShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const TaperedCylinderShapeSettings &, Shape::ShapeResult &>(), "settings"_a, "result"_a)
        .def("get_top_radius", &TaperedCylinderShape::GetTopRadius,
            "Get top radius of the tapered cylinder")
        .def("get_bottom_radius", &TaperedCylinderShape::GetBottomRadius,
            "Get bottom radius of the tapered cylinder")
        .def("get_convex_radius", &TaperedCylinderShape::GetConvexRadius,
            "Get convex radius of the tapered cylinder")
        .def("get_half_height", &TaperedCylinderShape::GetHalfHeight,
            "Get half height of the tapered cylinder")
        .def("get_center_of_mass", &TaperedCylinderShape::GetCenterOfMass)
        .def("get_local_bounds", &TaperedCylinderShape::GetLocalBounds)
        .def("get_inner_radius", &TaperedCylinderShape::GetInnerRadius)
        .def("get_mass_properties", &TaperedCylinderShape::GetMassProperties)
        .def("get_surface_normal", &TaperedCylinderShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &TaperedCylinderShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &TaperedCylinderShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("collide_point", &TaperedCylinderShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &TaperedCylinderShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &TaperedCylinderShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &TaperedCylinderShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("draw", &TaperedCylinderShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("save_binary_state", &TaperedCylinderShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &TaperedCylinderShape::GetStats)
        .def("get_volume", &TaperedCylinderShape::GetVolume)
        .def("is_valid_scale", &TaperedCylinderShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &TaperedCylinderShape::MakeScaleValid, "scale"_a)
        .def_static("register", &TaperedCylinderShape::sRegister);
}