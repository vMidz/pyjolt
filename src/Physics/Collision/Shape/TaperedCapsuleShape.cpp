#include "Common.h"
#include <Jolt/Physics/Collision/Shape/TaperedCapsuleShape.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

void BindTaperedCapsuleShape(nb::module_ &m) {
    nb::class_<TaperedCapsuleShapeSettings, ConvexShapeSettings> taperedCapsuleShapeSettingsCls(m, "TaperedCapsuleShapeSettings",
        "Class that constructs a TaperedCapsuleShape",
        nb::is_final());
    taperedCapsuleShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<float, float, float, const PhysicsMaterial *>(), "half_height_of_tapered_cylinder"_a, "top_radius"_a, "bottom_radius"_a, "material"_a = nullptr,
            "Create a tapered capsule centered around the origin with one sphere cap at (0, -inHalfHeightOfTaperedCylinder, 0) with radius inBottomRadius and the other at (0, inHalfHeightOfTaperedCylinder, 0) with radius inTopRadius")
        .def("is_valid", &TaperedCapsuleShapeSettings::IsValid,
            "Check if the settings are valid")
        .def("is_sphere", &TaperedCapsuleShapeSettings::IsSphere,
            "Checks if the settings of this tapered capsule make this shape a sphere")
        .def("create", &TaperedCapsuleShapeSettings::Create)
        .def_rw("half_height_of_tapered_cylinder", &TaperedCapsuleShapeSettings::mHalfHeightOfTaperedCylinder)
        .def_rw("top_radius", &TaperedCapsuleShapeSettings::mTopRadius)
        .def_rw("bottom_radius", &TaperedCapsuleShapeSettings::mBottomRadius);

    nb::class_<TaperedCapsuleShape, ConvexShape> taperedCapsuleShapeCls(m, "TaperedCapsuleShape",
        "A capsule with different top and bottom radii",
        nb::is_final());
    taperedCapsuleShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const TaperedCapsuleShapeSettings &, Shape::ShapeResult &>(), "settings"_a, "result"_a)
        .def("get_top_radius", &TaperedCapsuleShape::GetTopRadius,
            "Get top radius of the tapered capsule")
        .def("get_bottom_radius", &TaperedCapsuleShape::GetBottomRadius,
            "Get bottom radius of the tapered capsule")
        .def("get_half_height", &TaperedCapsuleShape::GetHalfHeight,
            "Get half height between the top and bottom sphere center")
        .def("get_center_of_mass", &TaperedCapsuleShape::GetCenterOfMass)
        .def("get_local_bounds", &TaperedCapsuleShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&TaperedCapsuleShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &TaperedCapsuleShape::GetInnerRadius)
        .def("get_mass_properties", &TaperedCapsuleShape::GetMassProperties)
        .def("get_surface_normal", &TaperedCapsuleShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &TaperedCapsuleShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &TaperedCapsuleShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("collide_soft_body_vertices", &TaperedCapsuleShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("draw", &TaperedCapsuleShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("save_binary_state", &TaperedCapsuleShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &TaperedCapsuleShape::GetStats)
        .def("get_volume", &TaperedCapsuleShape::GetVolume)
        .def("is_valid_scale", &TaperedCapsuleShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &TaperedCapsuleShape::MakeScaleValid, "scale"_a)
        .def_static("register", &TaperedCapsuleShape::sRegister);
}