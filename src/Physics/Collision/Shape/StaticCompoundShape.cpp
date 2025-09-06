#include "Common.h"
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <nanobind/ndarray.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Core/TempAllocator.h>

void BindStaticCompoundShape(nb::module_ &m) {
    nb::class_<StaticCompoundShapeSettings, CompoundShapeSettings> staticCompoundShapeSettingsCls(m, "StaticCompoundShapeSettings",
        "Class that constructs a StaticCompoundShape. Note that if you only want a compound of 1 shape, use a RotatedTranslatedShape instead.",
        nb::is_final());
    staticCompoundShapeSettingsCls
        .def(nb::init<>())
        .def("create", nb::overload_cast<>(&StaticCompoundShapeSettings::Create, nb::const_))
        .def("create", nb::overload_cast<TempAllocator &>(&StaticCompoundShapeSettings::Create, nb::const_), "temp_allocator"_a,
            "Specialization of Create() function that allows specifying a temp allocator to avoid temporary memory allocations on the heap");

    nb::class_<StaticCompoundShape, CompoundShape> staticCompoundShapeCls(m, "StaticCompoundShape",
        "A compound shape, sub shapes can be rotated and translated.\n"
        "Sub shapes cannot be modified once the shape is constructed.\n"
        "Shifts all child objects so that they're centered around the center of mass.",
        nb::is_final());
    staticCompoundShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const StaticCompoundShapeSettings &, TempAllocator &, Shape::ShapeResult &>(), "settings"_a, "temp_allocator"_a, "result"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&StaticCompoundShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&StaticCompoundShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &StaticCompoundShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collect_transformed_shapes", &StaticCompoundShape::CollectTransformedShapes, "box"_a, "position_com"_a, "rotation"_a, "scale"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("get_intersecting_sub_shapes", [](const StaticCompoundShape &self, const AABox &inBox, int inMaxSubShapeIndices){
            Array<uint32_t> indices_vec(inMaxSubShapeIndices);
            int count = self.GetIntersectingSubShapes(inBox, indices_vec.data(), inMaxSubShapeIndices);
            indices_vec.resize(count);
            return indices_vec;
        }, "box"_a, "max_sub_shape_indices"_a)
        .def("get_intersecting_sub_shapes", [](const StaticCompoundShape &self, const OrientedBox &inBox, int inMaxSubShapeIndices){
            Array<uint32_t> indices_vec(inMaxSubShapeIndices);
            int count = self.GetIntersectingSubShapes(inBox, indices_vec.data(), inMaxSubShapeIndices);
            indices_vec.resize(count);
            return indices_vec;
        }, "box"_a, "max_sub_shape_indices"_a)
        .def("save_binary_state", &StaticCompoundShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &StaticCompoundShape::GetStats)
        .def_static("register", &StaticCompoundShape::sRegister);
}