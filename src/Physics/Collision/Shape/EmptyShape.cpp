#include "Common.h"
#include <Jolt/Physics/Collision/Shape/EmptyShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

void BindEmptyShape(nb::module_ &m) {
    nb::class_<EmptyShapeSettings, ShapeSettings> emptyShapeSettingsCls(m, "EmptyShapeSettings",
        "Class that constructs an EmptyShape",
        nb::is_final());
    emptyShapeSettingsCls
        .def(nb::init<>())
        .def(nb::init<Vec3Arg>(), "center_of_mass"_a)
        .def("create", &EmptyShapeSettings::Create)
        .def_rw("center_of_mass", &EmptyShapeSettings::mCenterOfMass,
            "Determines the center of mass for this shape");

    nb::class_<EmptyShape, Shape> emptyShapeCls(m, "EmptyShape",
        "An empty shape that has no volume and collides with nothing.\n"
        "Possible use cases:\n"
        "- As a placeholder for a shape that will be created later. E.g. if you first need to create a body and only then know what shape it will have.\n"
        "- If you need a kinematic body to attach a constraint to, but you don't want the body to collide with anything.\n"
        "Note that, if possible, you should also put your body in an ObjectLayer that doesn't collide with anything.\n"
        "This ensures that collisions will be filtered out at broad phase level instead of at narrow phase level, this is more efficient.",
        nb::is_final());
    emptyShapeCls
        .def(nb::init<>())
        .def(nb::init<Vec3Arg>(), "center_of_mass"_a)
        .def(nb::init<const EmptyShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def("get_center_of_mass", &EmptyShape::GetCenterOfMass)
        .def("get_local_bounds", &EmptyShape::GetLocalBounds)
        .def("get_sub_shape_id_bits_recursive", &EmptyShape::GetSubShapeIDBitsRecursive)
        .def("get_inner_radius", &EmptyShape::GetInnerRadius)
        .def("get_mass_properties", &EmptyShape::GetMassProperties)
        .def("get_material", &EmptyShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_surface_normal", &EmptyShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_submerged_volume", [](const EmptyShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset) {
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
         }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
            "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &EmptyShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&EmptyShape::CastRay, nb::const_),
            "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &,
            CastRayCollector &, const ShapeFilter &>(&EmptyShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a,
            "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &EmptyShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &EmptyShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &EmptyShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &EmptyShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_stats", &EmptyShape::GetStats)
        .def("get_volume", &EmptyShape::GetVolume)
        .def("is_valid_scale", &EmptyShape::IsValidScale, "scale"_a)
        .def_static("register", &EmptyShape::sRegister);
}