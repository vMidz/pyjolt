#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/TransformedShape.h>

void BindScaledShape(nb::module_ &m) {
    nb::class_<ScaledShapeSettings, DecoratedShapeSettings> scaledShapeSettingsCls(m, "ScaledShapeSettings",
        "Class that constructs a ScaledShape",
        nb::is_final());
    scaledShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<const ShapeSettings *, Vec3Arg>(), "shape"_a, "scale"_a,
            "Constructor that decorates another shape with a scale")
        .def(nb::init<const Shape *, Vec3Arg>(), "shape"_a, "scale"_a,
            "Variant that uses a concrete shape, which means this object cannot be serialized.")
        .def("create", &ScaledShapeSettings::Create)
        .def_rw("scale", &ScaledShapeSettings::mScale);

    nb::class_<ScaledShape, DecoratedShape> scaledShapeCls(m, "ScaledShape",
        "A shape that scales a child shape in local space of that shape. The scale can be non-uniform and can even turn it inside out when one or three components of the scale are negative.",
        nb::is_final());
    scaledShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const ScaledShapeSettings &, Shape::ShapeResult &>(), "settings"_a, "result"_a)
        .def(nb::init<const Shape *, Vec3Arg>(), "shape"_a, "scale"_a,
            "Constructor that decorates another shape with a scale")
        .def("get_scale", &ScaledShape::GetScale,
            "Get the scale")
        .def("get_center_of_mass", &ScaledShape::GetCenterOfMass)
        .def("get_local_bounds", &ScaledShape::GetLocalBounds)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&ScaledShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &ScaledShape::GetInnerRadius)
        .def("get_mass_properties", &ScaledShape::GetMassProperties)
        .def("get_sub_shape_transformed_shape", &ScaledShape::GetSubShapeTransformedShape, "sub_shape_id"_a, "position_com"_a, "rotation"_a, "scale"_a, "remainder"_a)
        .def("get_surface_normal", &ScaledShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &ScaledShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_submerged_volume", [](ScaledShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,
                outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("draw", &ScaledShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("draw_get_support_function", &ScaledShape::DrawGetSupportFunction, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "draw_support_direction"_a)
        .def("draw_get_supporting_face", &ScaledShape::DrawGetSupportingFace, "renderer"_a, "center_of_mass_transform"_a, "scale"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&ScaledShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&ScaledShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &ScaledShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &ScaledShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("collect_transformed_shapes", &ScaledShape::CollectTransformedShapes, "box"_a, "position_com"_a, "rotation"_a, "scale"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("transform_shape", &ScaledShape::TransformShape, "center_of_mass_transform"_a, "collector"_a)
        .def("get_triangles_start", &ScaledShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &ScaledShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("save_binary_state", &ScaledShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &ScaledShape::GetStats)
        .def("get_volume", &ScaledShape::GetVolume)
        .def("is_valid_scale", &ScaledShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &ScaledShape::MakeScaleValid, "scale"_a)
        .def_static("register", &ScaledShape::sRegister);
}