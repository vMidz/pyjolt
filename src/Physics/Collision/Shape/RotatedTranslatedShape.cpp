#include "Common.h"
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

void BindRotatedTranslatedShape(nb::module_ &m) {
    nb::class_<RotatedTranslatedShapeSettings, DecoratedShapeSettings> rotatedTranslatedShapeSettingsCls(m, "RotatedTranslatedShapeSettings",
        "Class that constructs a RotatedTranslatedShape",
        nb::is_final());
    rotatedTranslatedShapeSettingsCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<Vec3Arg, QuatArg, const ShapeSettings *>(), "position"_a, "rotation"_a, "shape"_a,
            "Construct with shape settings, can be serialized.")
        .def(nb::init<Vec3Arg, QuatArg, const Shape *>(), "position"_a, "rotation"_a, "shape"_a,
            "Variant that uses a concrete shape, which means this object cannot be serialized.")
        .def("create", &RotatedTranslatedShapeSettings::Create)
        .def_rw("position", &RotatedTranslatedShapeSettings::mPosition,
            "Position of the sub shape")
        .def_rw("rotation", &RotatedTranslatedShapeSettings::mRotation,
            "Rotation of the sub shape");

    nb::class_<RotatedTranslatedShape, DecoratedShape> rotatedTranslatedShapeCls(m, "RotatedTranslatedShape",
        "A rotated translated shape will rotate and translate a child shape.\n"
        "Shifts the child object so that it is centered around the center of mass.",
        nb::is_final());
    rotatedTranslatedShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const RotatedTranslatedShapeSettings &, Shape::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<Vec3Arg, QuatArg , const Shape *>(), "position"_a, "rotation"_a, "shape"_a)
        .def("get_rotation", &RotatedTranslatedShape::GetRotation,
            "Access the rotation that is applied to the inner shape")
        .def("get_position", &RotatedTranslatedShape::GetPosition,
            "Access the translation that has been applied to the inner shape")
        .def("get_center_of_mass", &RotatedTranslatedShape::GetCenterOfMass)
        .def("get_local_bounds", &RotatedTranslatedShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&RotatedTranslatedShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &RotatedTranslatedShape::GetInnerRadius)
        .def("get_mass_properties", &RotatedTranslatedShape::GetMassProperties)
        .def("get_sub_shape_transformed_shape", &RotatedTranslatedShape::GetSubShapeTransformedShape, "sub_shape_id"_a, "position_com"_a, "rotation"_a, "scale"_a, "remainder"_a)
        .def("get_surface_normal", &RotatedTranslatedShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &RotatedTranslatedShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_submerged_volume", [](RotatedTranslatedShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,
                outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);

        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &RotatedTranslatedShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("draw_get_support_function", &RotatedTranslatedShape::DrawGetSupportFunction, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "draw_support_direction"_a)
        .def("draw_get_supporting_face", &RotatedTranslatedShape::DrawGetSupportingFace, "renderer"_a, "center_of_mass_transform"_a, "scale"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&RotatedTranslatedShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&RotatedTranslatedShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &RotatedTranslatedShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &RotatedTranslatedShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("collect_transformed_shapes", &RotatedTranslatedShape::CollectTransformedShapes, "box"_a, "position_com"_a, "rotation"_a, "scale"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("transform_shape", &RotatedTranslatedShape::TransformShape, "center_of_mass_transform"_a, "collector"_a)
        .def("get_triangles_start", &RotatedTranslatedShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &RotatedTranslatedShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &RotatedTranslatedShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &RotatedTranslatedShape::GetStats)
        .def("get_volume", &RotatedTranslatedShape::GetVolume)
        .def("is_valid_scale", &RotatedTranslatedShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &RotatedTranslatedShape::MakeScaleValid, "scale"_a)
        .def("transform_scale", &RotatedTranslatedShape::TransformScale, "scale"_a,
            "Transform the scale to the local space of the child shape")
        .def_static("register", &RotatedTranslatedShape::sRegister);
}