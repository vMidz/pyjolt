#include "Common.h"
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

void BindOffsetCenterOfMassShape(nb::module_ &m) {
    nb::class_<OffsetCenterOfMassShapeSettings, DecoratedShapeSettings> offsetCenterOfMassShapeSettingsCls(m, "OffsetCenterOfMassShapeSettings",
        "Class that constructs an OffsetCenterOfMassShape",
        nb::is_final());
    offsetCenterOfMassShapeSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<Vec3Arg, const ShapeSettings *>(), "offset"_a, "shape"_a,
            "Construct with shape settings, can be serialized.")
        .def(nb::init<Vec3Arg, const Shape *>(), "offset"_a, "shape"_a,
            "Variant that uses a concrete shape, which means this object cannot be serialized.")
        .def("create", &OffsetCenterOfMassShapeSettings::Create)
        .def_rw("offset", &OffsetCenterOfMassShapeSettings::mOffset,
            "Offset to be applied to the center of mass of the child shape");

    nb::class_<OffsetCenterOfMassShape, DecoratedShape> offsetCenterOfMassShapeCls(m, "OffsetCenterOfMassShape",
        "This shape will shift the center of mass of a child shape, it can e.g. be used to lower the center of mass of an unstable object like a boat to make it stable",
        nb::is_final());
    offsetCenterOfMassShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const OffsetCenterOfMassShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<const Shape *, Vec3Arg>(), "shape"_a, "offset"_a)
        .def("get_offset", &OffsetCenterOfMassShape::GetOffset,
            "Access the offset that is applied to the center of mass")
        .def("get_center_of_mass", &OffsetCenterOfMassShape::GetCenterOfMass)
        .def("get_local_bounds", &OffsetCenterOfMassShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&OffsetCenterOfMassShape::GetWorldSpaceBounds, nb::const_),
            "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &OffsetCenterOfMassShape::GetInnerRadius)
        .def("get_mass_properties", &OffsetCenterOfMassShape::GetMassProperties)
        .def("get_sub_shape_transformed_shape", &OffsetCenterOfMassShape::GetSubShapeTransformedShape,
            "sub_shape_id"_a, "position_com"_a, "rotation"_a, "scale"_a, "remainder"_a)
        .def("get_surface_normal", &OffsetCenterOfMassShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &OffsetCenterOfMassShape::GetSupportingFace,
            "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_submerged_volume", [](OffsetCenterOfMassShape &self, Mat44Arg inCenterOfMassTransform,
            Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy ;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &OffsetCenterOfMassShape::Draw,
            "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("draw_get_support_function", &OffsetCenterOfMassShape::DrawGetSupportFunction,
            "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "draw_support_direction"_a)
        .def("draw_get_supporting_face", &OffsetCenterOfMassShape::DrawGetSupportingFace,
            "renderer"_a, "center_of_mass_transform"_a, "scale"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&OffsetCenterOfMassShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&OffsetCenterOfMassShape::CastRay, nb::const_),
                "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &OffsetCenterOfMassShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &OffsetCenterOfMassShape::CollideSoftBodyVertices,
            "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("collect_transformed_shapes", &OffsetCenterOfMassShape::CollectTransformedShapes,
            "box"_a, "position_com"_a, "rotation"_a, "scale"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("transform_shape", &OffsetCenterOfMassShape::TransformShape, "center_of_mass_transform"_a, "collector"_a)
        .def("get_triangles_start", &OffsetCenterOfMassShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &OffsetCenterOfMassShape::GetTrianglesNext,
        //     "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &OffsetCenterOfMassShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &OffsetCenterOfMassShape::GetStats)
        .def("get_volume", &OffsetCenterOfMassShape::GetVolume)
        .def_static("register", &OffsetCenterOfMassShape::sRegister);
}