#include "Common.h"
#include <Jolt/Physics/Collision/Shape/DecoratedShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexShape.h>

void BindDecoratedShape(nb::module_ &m) {
    nb::class_<DecoratedShapeSettings, ShapeSettings> decoratedShapeSettingsCls(m, "DecoratedShapeSettings",
        "Class that constructs a DecoratedShape");
    decoratedShapeSettingsCls
        // .def(nb::init<>(),
        //     "Default constructor for deserialization")
        // .def(nb::init<const ShapeSettings *>(), "shape"_a,
        //     "Constructor that decorates another shape")
        // .def(nb::init<const Shape *>(), "shape"_a)
        .def_rw("inner_shape", &DecoratedShapeSettings::mInnerShape,
            "Sub shape (either this or mShapePtr needs to be filled up)")
        .def_rw("inner_shape_ptr", &DecoratedShapeSettings::mInnerShapePtr,
            "Sub shape (either this or mShape needs to be filled up)")
        .def("__repr__", [](const DecoratedShapeSettings &self) {
            std::string inner_repr = "None";
            if (self.mInnerShape)
                inner_repr = "<ShapeSettings>";
            else if (self.mInnerShapePtr)
                inner_repr = "<Shape>";
            return nb::str("DecoratedShapeSettings(inner={})").format(inner_repr);
        });

    nb::class_<DecoratedShape, Shape> decoratedShapeCls(m, "DecoratedShape",
        "Base class for shapes that decorate another shape with extra functionality (e.g. scale, translation etc.)");
    decoratedShapeCls
        // .def(nb::init<EShapeSubType>(), "sub_type"_a,
            // "Constructor")
        // .def(nb::init<EShapeSubType, const Shape *>(), "sub_type"_a, "inner_shape"_a)
        // .def(nb::init<EShapeSubType, const DecoratedShapeSettings &, ShapeSettings::ShapeResult &>(), "sub_type"_a, "settings"_a, "result"_a)
        // .def("get_inner_shape", &DecoratedShape::GetInnerShape, nb::rv_policy::reference,
            // "Access to the decorated inner shape")
        .def("must_be_static", &DecoratedShape::MustBeStatic)
        .def("get_center_of_mass", &DecoratedShape::GetCenterOfMass)
        .def("get_sub_shape_id_bits_recursive", &DecoratedShape::GetSubShapeIDBitsRecursive)
        .def("get_leaf_shape", &DecoratedShape::GetLeafShape, "sub_shape_id"_a, "remainder"_a, nb::rv_policy::reference)
        .def("get_material", &DecoratedShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_supporting_face", &DecoratedShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_sub_shape_user_data", &DecoratedShape::GetSubShapeUserData, "sub_shape_id"_a)
        .def("save_sub_shape_state", &DecoratedShape::SaveSubShapeState, "sub_shapes"_a)
        .def("restore_sub_shape_state", &DecoratedShape::RestoreSubShapeState, "sub_shapes"_a, "num_shapes"_a)
        .def("get_stats_recursive", &DecoratedShape::GetStatsRecursive, "visited_shapes"_a)
        .def("is_valid_scale", &DecoratedShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &DecoratedShape::MakeScaleValid, "scale"_a);
}