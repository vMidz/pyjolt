#include "Common.h"
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/Geometry/OrientedBox.h>

#include <nanobind/stl/vector.h>

void BindMutableCompoundShape(nb::module_ &m) {
    nb::class_<MutableCompoundShapeSettings, CompoundShapeSettings> mutableCompoundShapeSettingsCls(m, "MutableCompoundShapeSettings",
        "Class that constructs a MutableCompoundShape.",
        nb::is_final());
    mutableCompoundShapeSettingsCls
        .def(nb::init<>())
        .def("create", &MutableCompoundShapeSettings::Create);

    nb::class_<MutableCompoundShape, CompoundShape> mutableCompoundShapeCls(m, "MutableCompoundShape",
        "A compound shape, sub shapes can be rotated and translated.\n"
        "This shape is optimized for adding / removing and changing the rotation / translation of sub shapes but is less efficient in querying.\n"
        "Shifts all child objects so that they're centered around the center of mass (which needs to be kept up to date by calling AdjustCenterOfMass).\n"
        "Note: If you're using MutableCompoundShape and are querying data while modifying the shape you'll have a race condition.\n"
        "In this case it is best to create a new MutableCompoundShape using the Clone function. You replace the shape on a body using BodyInterface::SetShape.\n"
        "If a query is still working on the old shape, it will have taken a reference and keep the old shape alive until the query finishes.",
        nb::is_final());
    mutableCompoundShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const MutableCompoundShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def("clone", &MutableCompoundShape::Clone,
            "Clone this shape. Can be used to avoid race conditions. See the documentation of this class for more information.")
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&MutableCompoundShape::CastRay, nb::const_),
            "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &,
            CastRayCollector &, const ShapeFilter &>(&MutableCompoundShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &MutableCompoundShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collect_transformed_shapes", &MutableCompoundShape::CollectTransformedShapes,
            "box"_a, "position_com"_a, "rotation"_a, "scale"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("get_intersecting_sub_shapes", [](const MutableCompoundShape &self, const AABox &box, int max_sub_shape_indices) {
            std::vector<uint32> indices_vec(max_sub_shape_indices);
            int count = self.GetIntersectingSubShapes(box, indices_vec.data(), max_sub_shape_indices);
            indices_vec.resize(count);
            return indices_vec;
        }, "box"_a, "max_sub_shape_indices"_a)
        .def("get_intersecting_sub_shapes", [](const MutableCompoundShape &self, const OrientedBox &box, int max_sub_shape_indices) {
            std::vector<uint32> indices_vec(max_sub_shape_indices);
            int count = self.GetIntersectingSubShapes(box, indices_vec.data(), max_sub_shape_indices);
            indices_vec.resize(count);
            return indices_vec;
        }, "oriented_box"_a, "max_sub_shape_indices"_a)
        .def("save_binary_state", &MutableCompoundShape::SaveBinaryState, "stream"_a)
        .def("get_stats", &MutableCompoundShape::GetStats)
        .def("add_shape", &MutableCompoundShape::AddShape, "position"_a, "rotation"_a, "shape"_a, "user_data"_a = 0,
            "Adding a new shape.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.\n"
            "Returns:\n"
            "    uint: The index of the newly added shape.")
        .def("remove_shape", &MutableCompoundShape::RemoveShape, "index"_a,
            "Remove a shape by index.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.")
        .def("modify_shape", nb::overload_cast<uint, Vec3Arg, QuatArg>(&MutableCompoundShape::ModifyShape), "index"_a, "position"_a, "rotation"_a,
            "Modify the position / orientation of a shape.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.")
        .def("modify_shape", nb::overload_cast<uint, Vec3Arg, QuatArg, const Shape *>(&MutableCompoundShape::ModifyShape), "index"_a, "position"_a, "rotation"_a, "shape"_a,
            "Modify the position / orientation and shape at the same time.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.")
        .def("modify_shapes", [](MutableCompoundShape &self, uint start_index, Array<Vec3>&positions, Array<Quat> &rotations) {
            if (positions.size() != rotations.size())
                throw nb::value_error("Positions and rotations arrays must have the same number of elements.");

            self.ModifyShapes(start_index, positions.size(), positions.data(), rotations.data(), sizeof(Vec3), sizeof(Quat));
         }, "start_index"_a, "positions"_a, "rotations"_a,
            "@brief Batch set positions / orientations, this avoids duplicate work due to bounding box calculation.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.\n"
            "Args:\n"
            "    start_index (int): Index of first shape to update.\n"
            "    number (int): Number of shapes to update.\n"
            "    positions (Vec3*): A list of positions with arbitrary stride.\n"
            "    rotations (Quat*): A list of orientations with arbitrary stride.\n"
            "    position_stride (int): The position stride (the number of bytes between the first and second element).\n"
            "    rotation_stride (int): The orientation stride (the number of bytes between the first and second element).")
        .def("adjust_center_of_mass", &MutableCompoundShape::AdjustCenterOfMass,
            "Recalculate the center of mass and shift all objects so they're centered around it\n"
            "(this needs to be done of dynamic bodies and if the center of mass changes significantly due to adding / removing / repositioning sub shapes or else the simulation will look unnatural)\n"
            "Note that after adjusting the center of mass of an object you need to call BodyInterface::NotifyShapeChanged and Constraint::NotifyShapeChanged on the relevant bodies / constraints.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.")
        .def_static("register", &MutableCompoundShape::sRegister);
}