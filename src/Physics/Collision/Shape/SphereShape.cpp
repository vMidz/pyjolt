#include "Common.h"
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindSphereShape(nb::module_ &m) {
    nb::class_<SphereShapeSettings, ConvexShapeSettings> sphereShapeSettingsCls(m, "SphereShapeSettings",
        "Class that constructs a SphereShape",
        nb::is_final());
    sphereShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<float, const PhysicsMaterial *>(), "radius"_a, "material"_a = nullptr,
            "Create a sphere with radius inRadius")
        .def("create", &SphereShapeSettings::Create)
        .def_rw("radius", &SphereShapeSettings::mRadius);

    nb::class_<SphereShape, ConvexShape> sphereShapeCls(m, "SphereShape",
        "A sphere, centered around the origin.\n"
        "Note that it is implemented as a point with convex radius.",
        nb::is_final());
    sphereShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const SphereShapeSettings &, Shape::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<float, const PhysicsMaterial *>(), "radius"_a, "material"_a = nullptr,
            "Create a sphere with radius inRadius")
        .def("get_radius", &SphereShape::GetRadius,
            "Radius of the sphere")
        .def("get_local_bounds", &SphereShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&SphereShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &SphereShape::GetInnerRadius)
        .def("get_mass_properties", &SphereShape::GetMassProperties)
        .def("get_surface_normal", &SphereShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &SphereShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_support_function", &SphereShape::GetSupportFunction, "mode"_a, "buffer"_a, "scale"_a, nb::rv_policy::reference)
        .def("get_submerged_volume", [](SphereShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,
                outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &SphereShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&SphereShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&SphereShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &SphereShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &SphereShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &SphereShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &SphereShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &SphereShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &SphereShape::GetStats)
        .def("get_volume", &SphereShape::GetVolume)
        .def("is_valid_scale", &SphereShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &SphereShape::MakeScaleValid, "scale"_a)
        .def_static("register", &SphereShape::sRegister);
}