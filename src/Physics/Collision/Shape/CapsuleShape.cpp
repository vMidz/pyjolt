#include "Common.h"
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>

void BindCapsuleShape(nb::module_ &m) {
    nb::class_<CapsuleShapeSettings, ConvexShapeSettings> capsuleShapeSettingsCls(m, "CapsuleShapeSettings",
        "Class that constructs a CapsuleShape",
        nb::is_final());
    capsuleShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<float, float, const PhysicsMaterial *>(), "half_height_of_cylinder"_a, "radius"_a, "material"_a = nullptr,
            "Create a capsule centered around the origin with one sphere cap at (0, -inHalfHeightOfCylinder, 0) and the other at (0, inHalfHeightOfCylinder, 0)")
        .def("is_valid", &CapsuleShapeSettings::IsValid,
            "Check if this is a valid capsule shape")
        .def("is_sphere", &CapsuleShapeSettings::IsSphere,
            "Checks if the settings of this capsule make this shape a sphere")
        .def("create", &CapsuleShapeSettings::Create)
        .def_rw("radius", &CapsuleShapeSettings::mRadius)
        .def_rw("half_height_of_cylinder", &CapsuleShapeSettings::mHalfHeightOfCylinder);

    nb::class_<CapsuleShape, ConvexShape> capsuleShapeCls(m, "CapsuleShape",
        "A capsule, implemented as a line segment with convex radius",
        nb::is_final());
    capsuleShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const CapsuleShapeSettings &, CapsuleShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<float, float, const PhysicsMaterial *>(), "half_height_of_cylinder"_a, "radius"_a, "material"_a = nullptr,
            "Create a capsule centered around the origin with one sphere cap at (0, -inHalfHeightOfCylinder, 0) and the other at (0, inHalfHeightOfCylinder, 0)")
        .def("get_radius", &CapsuleShape::GetRadius,
            "Radius of the cylinder")
        .def("get_half_height_of_cylinder", &CapsuleShape::GetHalfHeightOfCylinder,
            "Get half of the height of the cylinder")
        .def("get_local_bounds", &CapsuleShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&CapsuleShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &CapsuleShape::GetInnerRadius)
        .def("get_mass_properties", &CapsuleShape::GetMassProperties)
        .def("get_surface_normal", &CapsuleShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &CapsuleShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &CapsuleShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("draw", &CapsuleShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&CapsuleShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("collide_point", &CapsuleShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &CapsuleShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &CapsuleShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &CapsuleShape::GetTrianglesNext,
            // "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &CapsuleShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &CapsuleShape::GetStats)
        .def("get_volume", &CapsuleShape::GetVolume)
        .def("is_valid_scale", &CapsuleShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &CapsuleShape::MakeScaleValid, "scale"_a)
        .def_static("register", &CapsuleShape::sRegister);
}