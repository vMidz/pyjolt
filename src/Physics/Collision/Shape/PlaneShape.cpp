#include "Common.h"
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>
#include <Jolt/Physics/Collision/Shape/ScaleHelpers.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/ObjectStream/TypeDeclarations.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

void BindPlaneShape(nb::module_ &m) {
    nb::class_<PlaneShapeSettings, ShapeSettings> planeShapeSettingsCls(m, "PlaneShapeSettings",
        "Class that constructs a PlaneShape",
        nb::is_final());
    planeShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<const Plane &, const PhysicsMaterial *, float>(), "plane"_a, "material"_a = nullptr, "half_extent"_a = PlaneShapeSettings::cDefaultHalfExtent,
            "Create a plane shape.")
        .def("create", &PlaneShapeSettings::Create)
        .def_rw("plane", &PlaneShapeSettings::mPlane,
            "Plane that describes the shape. The negative half space is considered solid.")
        .def_rw("material", &PlaneShapeSettings::mMaterial,
            "Surface material of the plane")
        .def_ro_static("C_DEFAULT_HALF_EXTENT", &PlaneShapeSettings::cDefaultHalfExtent,
            "Default half-extent of the plane (total size along 1 axis will be 2 * half-extent)")
        .def_rw("half_extent", &PlaneShapeSettings::mHalfExtent,
            "The bounding box of this plane will run from [-half_extent, half_extent]. Keep this as low as possible for better broad phase performance.");

    nb::class_<PlaneShape, Shape> planeShapeCls(m, "PlaneShape",
        "A plane shape. The negative half space is considered solid. Planes cannot be dynamic objects, only static or kinematic.\n"
        "The plane is considered an infinite shape, but testing collision outside of its bounding box (defined by the half-extent parameter) will not return a collision result.\n"
        "At the edge of the bounding box collision with the plane will be inconsistent. If you need something of a well defined size, a box shape may be better.",
        nb::is_final());
    planeShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const Plane &, const PhysicsMaterial *, float>(), "plane"_a, "material"_a = nullptr, "half_extent"_a = PlaneShapeSettings::cDefaultHalfExtent)
        .def(nb::init<const PlaneShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def("get_plane", &PlaneShape::GetPlane,
            "Get the plane")
        .def("get_half_extent", &PlaneShape::GetHalfExtent,
            "Get the half-extent of the bounding box of the plane")
        .def("must_be_static", &PlaneShape::MustBeStatic)
        .def("get_local_bounds", &PlaneShape::GetLocalBounds)
        .def("get_sub_shape_id_bits_recursive", &PlaneShape::GetSubShapeIDBitsRecursive)
        .def("get_inner_radius", &PlaneShape::GetInnerRadius)
        .def("get_mass_properties", &PlaneShape::GetMassProperties)
        .def("get_material", &PlaneShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_surface_normal", &PlaneShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &PlaneShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("draw", &PlaneShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&PlaneShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray",nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&PlaneShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &PlaneShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &PlaneShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &PlaneShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &PlaneShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_submerged_volume",  [](PlaneShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,
                outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("save_binary_state", &PlaneShape::SaveBinaryState, "stream"_a)
        .def("save_material_state", &PlaneShape::SaveMaterialState, "materials"_a)
        .def("restore_material_state", &PlaneShape::RestoreMaterialState, "materials"_a, "num_materials"_a)
        .def("get_stats", &PlaneShape::GetStats)
        .def("get_volume", &PlaneShape::GetVolume)
        .def_static("register", &PlaneShape::sRegister)
        .def("__repr__", [](const PlaneShape &self) {
            auto physicsMat = nb::repr(nb::find(self.GetMaterial({})));
            return nb::str("PlaneShape(plane={}, half_extent={}, material={})")
                .format(0, self.GetHalfExtent(), physicsMat.c_str());
        });

}