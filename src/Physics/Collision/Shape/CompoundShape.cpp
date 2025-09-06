#include "Common.h"

#include <Jolt/Physics/Collision/Shape/CompoundShape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/ObjectStream/TypeDeclarations.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

#include <nanobind/stl/vector.h>
#include <nanobind/stl/array.h>

void BindCompoundShape(nb::module_ &m) {
    nb::class_<CompoundShapeSettings, ShapeSettings> compoundShapeSettingsCls(m, "CompoundShapeSettings",
        "Base class settings to construct a compound shape");
    compoundShapeSettingsCls
        // .def(nb::init<>(),
            // "Constructor. Use AddShape to add the parts.")
        .def("add_shape", nb::overload_cast<Vec3Arg, QuatArg, const ShapeSettings *, uint32>(&CompoundShapeSettings::AddShape),
            "position"_a, "rotation"_a, "shape"_a, "user_data"_a = 0, "Add a shape to the compound.")
        .def("add_shape", nb::overload_cast<Vec3Arg, QuatArg, const Shape *, uint32>(&CompoundShapeSettings::AddShape), "position"_a, "rotation"_a, "shape"_a, "user_data"_a = 0,
            "Add a shape to the compound. Variant that uses a concrete shape, which means this object cannot be serialized.")
        .def_rw("sub_shapes", &CompoundShapeSettings::mSubShapes);

    nb::class_<CompoundShapeSettings::SubShapeSettings> subShapeSettingsCls(compoundShapeSettingsCls, "SubShapeSettings");
    subShapeSettingsCls
        .def_rw("shape", &CompoundShapeSettings::SubShapeSettings::mShape,
            "Sub shape (either this or mShapePtr needs to be filled up)")
        .def_rw("shape_ptr", &CompoundShapeSettings::SubShapeSettings::mShapePtr,
            "Sub shape (either this or mShape needs to be filled up)")
        .def_rw("position", &CompoundShapeSettings::SubShapeSettings::mPosition,
            "Position of the sub shape")
        .def_rw("rotation", &CompoundShapeSettings::SubShapeSettings::mRotation,
                    "Rotation of the sub shape")
        .def_rw("user_data", &CompoundShapeSettings::SubShapeSettings::mUserData,
            "User data value (can be used by the application for any purpose)");

    nb::class_<CompoundShape, Shape> compoundShapeCls(m, "CompoundShape",
        "Base class for a compound shape");
    compoundShapeCls
        // .def(nb::init<EShapeSubType>(), "sub_type"_a,
            // "Constructor")
        // .def(nb::init<EShapeSubType, const ShapeSettings &, ShapeSettings::ShapeResult &>(), "sub_type"_a, "settings"_a, "result"_a)
        .def("get_center_of_mass", &CompoundShape::GetCenterOfMass)
        .def("must_be_static", &CompoundShape::MustBeStatic)
        .def("get_local_bounds", &CompoundShape::GetLocalBounds)
        .def("get_sub_shape_id_bits_recursive", &CompoundShape::GetSubShapeIDBitsRecursive)
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&CompoundShape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a)
        .def("get_inner_radius", &CompoundShape::GetInnerRadius)
        .def("get_mass_properties", &CompoundShape::GetMassProperties)
        .def("get_material", &CompoundShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_leaf_shape", [](const CompoundShape &self, const SubShapeID &inSubShapeID) {
            SubShapeID outRemainder;
            const Shape *leaf = self.GetLeafShape(inSubShapeID, outRemainder);
            return nb::make_tuple(leaf, outRemainder);
        }, "sub_shape_id"_a, nb::rv_policy::reference, "Output: [leaf, outRemainder]")
        .def("get_sub_shape_user_data", &CompoundShape::GetSubShapeUserData, "sub_shape_id"_a)
        .def("get_sub_shape_transformed_shape", [](const CompoundShape &self, const SubShapeID &inSubShapeID, Vec3Arg inPositionCOM, QuatArg inRotation, Vec3Arg inScale) {
            SubShapeID outRemainder;
            TransformedShape ts = self.GetSubShapeTransformedShape(inSubShapeID, inPositionCOM, inRotation, inScale, outRemainder);
            return nb::make_tuple(ts, outRemainder);
        }, "sub_shape_id"_a, "position_com"_a, "rotation"_a, "scale"_a)
        .def("get_surface_normal", &CompoundShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &CompoundShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        // .def("get_submerged_volume", &CompoundShape::GetSubmergedVolume,
            // "center_of_mass_transform"_a, "scale"_a, "surface"_a, "total_volume"_a, "submerged_volume"_a, "center_of_buoyancy"_a)
        .def("draw", &CompoundShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("draw_get_support_function", &CompoundShape::DrawGetSupportFunction,
            "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "draw_support_direction"_a)
        .def("draw_get_supporting_face", &CompoundShape::DrawGetSupportingFace, "renderer"_a, "center_of_mass_transform"_a, "scale"_a)
        .def("collide_soft_body_vertices", &CompoundShape::CollideSoftBodyVertices,
            "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("transform_shape", &CompoundShape::TransformShape, "center_of_mass_transform"_a, "collector"_a)
        .def("get_triangles_start", &CompoundShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &CompoundShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_intersecting_sub_shapes", [](const CompoundShape &self, const AABox &inBox, int inMaxSubShapeIndices) {
            uint num_shapes = self.GetNumSubShapes();
            if (inMaxSubShapeIndices > num_shapes)
                inMaxSubShapeIndices = num_shapes; // Clamp max indices
            std::vector<uint> indices_buffer(inMaxSubShapeIndices);
            int count = self.GetIntersectingSubShapes(inBox, indices_buffer.data(), inMaxSubShapeIndices);
            indices_buffer.resize(count); // Resize to actual count
            return indices_buffer;
        }, "box"_a, "max_sub_shape_indices"_a,
            "Get which sub shape's bounding boxes overlap with an axis aligned box.\n"
            "Args:\n"
            "    box (AABox): The axis aligned box to test against (relative to the center of mass of this shape).\n"
            "    sub_shape_indices (uint*): Buffer where to place the indices of the sub shapes that intersect.\n"
            "    max_sub_shape_indices (int): How many indices will fit in the buffer (normally you'd provide a buffer of GetNumSubShapes() indices).\n"
            "Returns:\n"
            "    int: How many indices were placed in outSubShapeIndices.")
        .def("get_intersecting_sub_shapes",[](const CompoundShape &self, const OrientedBox &inBox, int inMaxSubShapeIndices) {
            uint num_shapes = self.GetNumSubShapes();
            if (inMaxSubShapeIndices > num_shapes)
                inMaxSubShapeIndices = num_shapes; // Clamp max indices
            std::vector<uint> indices_buffer(inMaxSubShapeIndices);
            int count = self.GetIntersectingSubShapes(inBox, indices_buffer.data(), inMaxSubShapeIndices);
            indices_buffer.resize(count);
            return indices_buffer;
        }, "box"_a, "sub_shape_indices"_a,
            "Get which sub shape's bounding boxes overlap with an axis aligned box.\n"
            "Args:\n"
            "    box (OrientedBox): The axis aligned box to test against (relative to the center of mass of this shape).\n"
            "    sub_shape_indices (uint*): Buffer where to place the indices of the sub shapes that intersect.\n"
            "    max_sub_shape_indices (int): How many indices will fit in the buffer (normally you'd provide a buffer of GetNumSubShapes() indices).\n"
            "Returns:\n"
            "    int: How many indices were placed in outSubShapeIndices.")
        .def("get_sub_shapes", &CompoundShape::GetSubShapes,
            "Access to the sub shapes of this compound")
        .def("get_num_sub_shapes", &CompoundShape::GetNumSubShapes,
            "Get the total number of sub shapes")
        .def("get_sub_shape", &CompoundShape::GetSubShape, "idx"_a,
            "Access to a particular sub shape")
        .def("get_compound_user_data", &CompoundShape::GetCompoundUserData, "idx"_a,
            "Get the user data associated with a shape in this compound")
        .def("set_compound_user_data", &CompoundShape::SetCompoundUserData, "idx"_a, "user_data"_a,
            "Set the user data associated with a shape in this compound")
        .def("is_sub_shape_id_valid", &CompoundShape::IsSubShapeIDValid, "sub_shape_id"_a,
            "Check if a sub shape ID is still valid for this shape.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): Sub shape id that indicates the leaf shape relative to this shape.\n"
            "Returns:\n"
            "    bool: True if the ID is valid, false if not.")
        .def("get_sub_shape_index_from_id", [](const CompoundShape &self, SubShapeID inSubShapeID) {
            SubShapeID outRemainder;
            uint32 index = self.GetSubShapeIndexFromID(inSubShapeID, outRemainder);
            return nb::make_tuple(index, outRemainder);
        }, "sub_shape_id"_a,
            "Convert SubShapeID to sub shape index.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): Sub shape id that indicates the leaf shape relative to this shape.\n"
            "    remainder (SubShapeID): This is the sub shape ID for the sub shape of the compound after popping off the index.\n"
            "Returns:\n"
            "    uint32: The index of the sub shape of this compound.")
        .def("get_sub_shape_id_from_index", &CompoundShape::GetSubShapeIDFromIndex, "idx"_a, "parent_sub_shape_id"_a,
            "@brief Convert a sub shape index to a sub shape ID.\n"
            "Args:\n"
            "    idx (int): Index of the sub shape of this compound.\n"
            "    parent_sub_shape_id (SubShapeIDCreator): Parent SubShapeID (describing the path to the compound shape).\n"
            "Returns:\n"
            "    SubShapeIDCreator: A sub shape ID creator that contains the full path to the sub shape with index inIdx.")
        .def("save_binary_state", &CompoundShape::SaveBinaryState, "stream"_a)
        .def("save_sub_shape_state", &CompoundShape::SaveSubShapeState, "sub_shapes"_a)
        .def("restore_sub_shape_state", &CompoundShape::RestoreSubShapeState, "sub_shapes"_a, "num_shapes"_a)
        .def("get_stats_recursive", &CompoundShape::GetStatsRecursive, "visited_shapes"_a)
        .def("get_volume", &CompoundShape::GetVolume)
        .def("is_valid_scale", &CompoundShape::IsValidScale, "scale"_a)
        .def("make_scale_valid", &CompoundShape::MakeScaleValid, "scale"_a)
        .def_static("register", &CompoundShape::sRegister)
        .def("__repr__", [](const CompoundShape &self) {
            return nb::str("<jolt.CompoundShape ({} sub-shapes)>").format(self.GetNumSubShapes());
        });

    nb::class_<CompoundShape::SubShape> subShapeCls(m, "SubShape");
    subShapeCls
        .def("from_settings", &CompoundShape::SubShape::FromSettings, "settings"_a, "result"_a,
            "Initialize sub shape from sub shape settings.\n"
            "Args:\n"
            "    settings (CompoundShapeSettings): Settings object.\n"
            "    result (ShapeResult): Result object, only used in case of error.\n"
            "Returns:\n"
            "    bool: True on success, false on failure.")
        .def("set_transform", &CompoundShape::SubShape::SetTransform, "position"_a, "rotation"_a, "center_of_mass"_a,
            "Update the transform of this sub shape.\n"
            "Args:\n"
            "    position (Vec3): New position.\n"
            "    rotation (Quat): New orientation.\n"
            "    center_of_mass (Vec3): The center of mass of the compound shape.")
        .def("get_local_transform_no_scale", &CompoundShape::SubShape::GetLocalTransformNoScale, "scale"_a,
            "Get the local transform for this shape given the scale of the child shape\n"
            "The total transform of the child shape will be GetLocalTransformNoScale(inScale) * Mat44::sScaling(TransformScale(inScale)).\n"
            "Args:\n"
            "    scale (Vec3): The scale of the child shape (in local space of this shape).")
        .def("is_valid_scale", &CompoundShape::SubShape::IsValidScale, "scale"_a,
        "Test if inScale is valid for this sub shape")
        .def("transform_scale", &CompoundShape::SubShape::TransformScale, "scale"_a,
            "Transform the scale to the local space of the child shape")
        .def("set_position_com", &CompoundShape::SubShape::SetPositionCOM, "position_com"_a,
            "Compress the center of mass position")
        .def("get_position_com", &CompoundShape::SubShape::GetPositionCOM,
            "Uncompress the center of mass position")
        .def("set_rotation", &CompoundShape::SubShape::SetRotation, "rotation"_a,
            "Compress the rotation")
        .def("get_rotation", &CompoundShape::SubShape::GetRotation,
            "Uncompress the rotation")
        .def_rw("shape", &CompoundShape::SubShape::mShape)
        .def_rw("position_com", &CompoundShape::SubShape::mPositionCOM,
            "Note: Position of center of mass of sub shape!")
        .def_rw("rotation", &CompoundShape::SubShape::mRotation,
            "Note: X, Y, Z of rotation quaternion - note we read 4 bytes beyond this so make sure there's something there")
        .def_rw("user_data", &CompoundShape::SubShape::mUserData,
            "User data value (put here because it falls in padding bytes)")
        .def_rw("is_rotation_identity", &CompoundShape::SubShape::mIsRotationIdentity,
            "If mRotation is close to identity (put here because it falls in padding bytes)");
}