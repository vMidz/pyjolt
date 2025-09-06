#include "Common.h"
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/Core/TempAllocator.h>

void BindHeightFieldShape(nb::module_ &m) {
    using namespace HeightFieldShapeConstants;
    m.attr("NO_COLLISION_VALUE") = cNoCollisionValue;
    m.attr("STACK_SIZE") = cStackSize;
    m.attr("NUM_BITS_XY") = cNumBitsXY;
    m.attr("MASK_BITS_XY") = cMaskBitsXY;
    m.attr("LEVEL_SHIFT") = cLevelShift;
    m.attr("NO_COLLISION_VALUE16") = cNoCollisionValue16;
    m.attr("MAX_HEIGHT_VALUE16") = cMaxHeightValue16;

    nb::class_<HeightFieldShapeSettings, ShapeSettings> heightFieldShapeSettingsCls(m, "HeightFieldShapeSettings",
        "Class that constructs a HeightFieldShape",
        nb::is_final());
    heightFieldShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def("__init__",[](HeightFieldShapeSettings &self, nb::list inSamples, Vec3Arg inOffset, Vec3Arg inScale, uint32 inSampleCount, nb::list inMaterialIndices, const PhysicsMaterialList &inMaterialList){
            std::vector<float> samples;
            for (const auto &e : inSamples)
                samples.push_back(nb::cast<float>(e));
        
            std::vector<uint8> indices;
            for(const auto &e : inMaterialIndices)
                indices.push_back(nb::cast<uint8>(e));

            new (&self) HeightFieldShapeSettings(samples.data(), inOffset, inScale, inSampleCount, indices.data(), inMaterialList);
         }, "samples"_a, "offset"_a, "scale"_a, "sample_count"_a,"material_indices"_a = nullptr, "inMaterialList"_a = PhysicsMaterialList (),
            "Create a height field shape of inSampleCount * inSampleCount vertices.\n"
            "The height field is a surface defined by: inOffset + inScale * (x, inSamples[y * inSampleCount + x], y).\n"
            "where x and y are integers in the range x and y e [0, inSampleCount - 1].\n"
            "inSampleCount: inSampleCount / mBlockSize must be minimally 2 and a power of 2 is the most efficient in terms of performance and storage.\n"
            "inSamples: inSampleCount^2 vertices.\n"
            "inMaterialIndices: (inSampleCount - 1)^2 indices that index into inMaterialList.")
        .def("create", &HeightFieldShapeSettings::Create)
        .def("determine_min_and_max_sample", &HeightFieldShapeSettings::DetermineMinAndMaxSample, "min_value"_a, "max_value"_a, "quantization_scale"_a,
            "Determine the minimal and maximal value of mHeightSamples (will ignore cNoCollisionValue).\n"
            "Args:\n"
            "    min_value (float): The minimal value of mHeightSamples or FLT_MAX if no samples have collision.\n"
            "    max_value (float): The maximal value of mHeightSamples or -FLT_MAX if no samples have collision.\n"
            "    quantization_scale (float): (value - outMinValue) * outQuantizationScale quantizes a height sample to 16 bits.")
        .def("calculate_bits_per_sample_for_error", &HeightFieldShapeSettings::CalculateBitsPerSampleForError, "max_error"_a,
            "Given mBlockSize, mSampleCount and mHeightSamples, calculate the amount of bits needed to stay below absolute error inMaxError.\n"
            "Args:\n"
            "    max_error (float): Maximum allowed error in mHeightSamples after compression (note that this does not take mScale.Y into account).\n"
            "Returns:\n"
            "    uint32: Needed bits per sample in the range [1, 8].")
        .def_rw("offset", &HeightFieldShapeSettings::mOffset,
            "The height field is a surface defined by: mOffset + mScale * (x, mHeightSamples[y * mSampleCount + x], y).\n"
            "where x and y are integers in the range x and y e [0, mSampleCount - 1].")
        .def_rw("scale", &HeightFieldShapeSettings::mScale)
        .def_rw("sample_count", &HeightFieldShapeSettings::mSampleCount)
        .def_rw("min_height_value", &HeightFieldShapeSettings::mMinHeightValue,
            "Artificial minimal value of mHeightSamples, used for compression and can be used to update the terrain after creating with lower height values. If there are any lower values in mHeightSamples, this value will be ignored.")
        .def_rw("max_height_value", &HeightFieldShapeSettings::mMaxHeightValue,
            "Artificial maximum value of mHeightSamples, used for compression and can be used to update the terrain after creating with higher height values. If there are any higher values in mHeightSamples, this value will be ignored.")
        .def_rw("materials_capacity", &HeightFieldShapeSettings::mMaterialsCapacity,
            "When bigger than mMaterials.size() the internal material list will be preallocated to support this number of materials.\n"
            "This avoids reallocations when calling HeightFieldShape::SetMaterials with new materials later.")
        .def_rw("block_size", &HeightFieldShapeSettings::mBlockSize,
            "The heightfield is divided in blocks of mBlockSize * mBlockSize * 2 triangles and the acceleration structure culls blocks only,\n"
            "bigger block sizes reduce memory consumption but also reduce query performance. Sensible values are [2, 8], does not need to be\n"
            "a power of 2. Note that at run-time we'll perform one more grid subdivision, so the effective block size is half of what is provided here.")
        .def_rw("bits_per_sample", &HeightFieldShapeSettings::mBitsPerSample,
            "How many bits per sample to use to compress the height field. Can be in the range [1, 8].\n"
            "Note that each sample is compressed relative to the min/max value of its block of mBlockSize * mBlockSize pixels so the effective precision is higher.\n"
            "Also note that increasing mBlockSize saves more memory than reducing the amount of bits per sample.")
        .def_rw("height_samples", &HeightFieldShapeSettings::mHeightSamples,
            "An array of mSampleCount^2 height samples. Samples are stored in row major order, so the sample at (x, y) is at index y * mSampleCount + x.")
        .def_rw("material_indices", &HeightFieldShapeSettings::mMaterialIndices,
            "An array of (mSampleCount - 1)^2 material indices.")
        .def_rw("materials", &HeightFieldShapeSettings::mMaterials,
            "The materials of square at (x, y) is: mMaterials[mMaterialIndices[x + y * (mSampleCount - 1)]]")
        .def_rw("active_edge_cos_threshold_angle", &HeightFieldShapeSettings::mActiveEdgeCosThresholdAngle,
            "Cosine of the threshold angle (if the angle between the two triangles is bigger than this, the edge is active, note that a concave edge is always inactive).\n"
            "Setting this value too small can cause ghost collisions with edges, setting it too big can cause depenetration artifacts (objects not depenetrating quickly).\n"
            "Valid ranges are between cos(0 degrees) and cos(90 degrees). The default value is cos(5 degrees).");

    nb::class_<HeightFieldShape, Shape> heightFieldShapeCls(m, "HeightFieldShape",
        "A height field shape. Cannot be used as a dynamic object.\n"
        "Note: If you're using HeightFieldShape and are querying data while modifying the shape you'll have a race condition.\n"
        "In this case it is best to create a new HeightFieldShape using the Clone function. You replace the shape on a body using BodyInterface::SetShape.\n"
        "If a query is still working on the old shape, it will have taken a reference and keep the old shape alive until the query finishes.",
        nb::is_final());
    heightFieldShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const HeightFieldShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def("clone", &HeightFieldShape::Clone,
            "Clone this shape. Can be used to avoid race conditions. See the documentation of this class for more information.")
        .def("must_be_static", &HeightFieldShape::MustBeStatic)
        .def("get_sample_count", &HeightFieldShape::GetSampleCount,
            "Get the size of the height field. Note that this will always be rounded up to the nearest multiple of GetBlockSize().")
        .def("get_block_size", &HeightFieldShape::GetBlockSize,
            "Get the size of a block")
        .def("get_local_bounds", &HeightFieldShape::GetLocalBounds)
        .def("get_sub_shape_id_bits_recursive", &HeightFieldShape::GetSubShapeIDBitsRecursive)
        .def("get_inner_radius", &HeightFieldShape::GetInnerRadius)
        .def("get_mass_properties", &HeightFieldShape::GetMassProperties)
        .def("get_material", nb::overload_cast<const SubShapeID &>(&HeightFieldShape::GetMaterial, nb::const_), "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_material", nb::overload_cast<uint, uint>(&HeightFieldShape::GetMaterial, nb::const_), "x"_a, "y"_a, nb::rv_policy::reference,
            "Overload to get the material at a particular location")
        .def("get_surface_normal", &HeightFieldShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", &HeightFieldShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a)
        .def("get_submerged_volume", [](HeightFieldShape &self,
                                        Mat44Arg inCenterOfMassTransform,
                                        Vec3Arg inScale,
                                        const Plane &inSurface,
                                        Vec3 inBaseOffset){

            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface,
            outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a)
        .def("draw", &HeightFieldShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)

        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &,
            RayCastResult &>(&HeightFieldShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &,
            CastRayCollector &, const ShapeFilter &>(&HeightFieldShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)

        .def("collide_point", &HeightFieldShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_soft_body_vertices", &HeightFieldShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &HeightFieldShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &HeightFieldShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_position", nb::overload_cast<uint, uint>(&HeightFieldShape::GetPosition, nb::const_), "x"_a, "y"_a,
            "Get height field position at sampled location (inX, inY).\n"
            "where inX and inY are integers in the range inX e [0, mSampleCount - 1] and inY e [0, mSampleCount - 1].")
        .def("is_no_collision", &HeightFieldShape::IsNoCollision, "x"_a, "y"_a,
            "Check if height field at sampled location (inX, inY) has collision (has a hole or not)")
        .def("project_onto_surface", &HeightFieldShape::ProjectOntoSurface, "local_position"_a, "surface_position"_a, "sub_shape_id"_a,
            "Projects inLocalPosition (a point in the space of the shape) along the Y axis onto the surface and returns it in outSurfacePosition.\n"
            "When there is no surface position (because of a hole or because the point is outside the heightfield) the function will return false.")
        .def("get_sub_shape_coordinates", &HeightFieldShape::GetSubShapeCoordinates, "sub_shape_id"_a, "x"_a, "y"_a, "triangle_index"_a,
            "Returns the coordinates of the triangle that a sub shape ID represents.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): The sub shape ID to decode.\n"
            "    x (int): X coordinate of the triangle (in the range [0, mSampleCount - 2]).\n"
            "    y (int): Y coordinate of the triangle (in the range [0, mSampleCount - 2]).\n"
            "    triangle_index (int): Triangle within the quad (0 = lower triangle or 1 = upper triangle).")
        .def("get_min_height_value", &HeightFieldShape::GetMinHeightValue,
            "Get the range of height values that this height field can encode. Can be used to determine the allowed range when setting the height values with SetHeights.")
        .def("get_max_height_value", &HeightFieldShape::GetMaxHeightValue)
        .def("get_heights", &HeightFieldShape::GetHeights, "x"_a, "y"_a, "size_x"_a, "size_y"_a, "heights"_a, "heights_stride"_a,
            "Get the height values of a block of data.\n"
            "Note that the height values are decompressed so will be slightly different from what the shape was originally created with.\n"
            "Args:\n"
            "    x (int): Start X position, must be a multiple of mBlockSize and in the range [0, mSampleCount - 1].\n"
            "    y (int): Start Y position, must be a multiple of mBlockSize and in the range [0, mSampleCount - 1].\n"
            "    size_x (int): Number of samples in X direction, must be a multiple of mBlockSize and in the range [0, mSampleCount - inX].\n"
            "    size_y (int): Number of samples in Y direction, must be a multiple of mBlockSize and in the range [0, mSampleCount - inY].\n"
            "    heights (float*): Returned height values, must be at least inSizeX * inSizeY floats. Values are returned in x-major order and can be cNoCollisionValue.\n"
            "    heights_stride (intptr_t): Stride in floats between two consecutive rows of outHeights (can be negative if the data is upside down).")
        .def("set_heights", &HeightFieldShape::SetHeights, "x"_a, "y"_a, "size_x"_a, "size_y"_a, "heights"_a, "heights_stride"_a, "allocator"_a, "active_edge_cos_threshold_angle"_a = 0.996195f,
            "Set the height values of a block of data.\n"
            "Note that this requires decompressing and recompressing a border of size mBlockSize in the negative x/y direction so will cause some precision loss.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.\n"
            "Args:\n"
            "    x (int): Start X position, must be a multiple of mBlockSize and in the range [0, mSampleCount - 1].\n"
            "    y (int): Start Y position, must be a multiple of mBlockSize and in the range [0, mSampleCount - 1].\n"
            "    size_x (int): Number of samples in X direction, must be a multiple of mBlockSize and in the range [0, mSampleCount - inX].\n"
            "    size_y (int): Number of samples in Y direction, must be a multiple of mBlockSize and in the range [0, mSampleCount - inY].\n"
            "    heights (float*): The new height values to set, must be an array of inSizeX * inSizeY floats, can be cNoCollisionValue. Values outside of the range [GetMinHeightValue(), GetMaxHeightValue()] will be clamped.\n"
            "    heights_stride (intptr_t): Stride in floats between two consecutive rows of inHeights (can be negative if the data is upside down).\n"
            "    allocator (TempAllocator): Allocator to use for temporary memory.\n"
            "    active_edge_cos_threshold_angle (float): Cosine of the threshold angle (if the angle between the two triangles is bigger than this, the edge is active, note that a concave edge is always inactive).")
        .def("get_material_list", &HeightFieldShape::GetMaterialList,
            "Get the current list of materials, the indices returned by GetMaterials() will index into this list.")
        .def("get_materials", &HeightFieldShape::GetMaterials, "x"_a, "y"_a, "size_x"_a, "size_y"_a, "materials"_a, "materials_stride"_a,
            "Get the material indices of a block of data.\n"
            "Args:\n"
            "    x (int): Start X position, must in the range [0, mSampleCount - 1].\n"
            "    y (int): Start Y position, must in the range [0, mSampleCount - 1].\n"
            "    size_x (int): Number of samples in X direction.\n"
            "    size_y (int): Number of samples in Y direction.\n"
            "    materials (uint8*): Returned material indices, must be at least inSizeX * inSizeY uint8s. Values are returned in x-major order.\n"
            "    materials_stride (intptr_t): Stride in uint8s between two consecutive rows of outMaterials (can be negative if the data is upside down).")
        .def("set_materials", &HeightFieldShape::SetMaterials, "x"_a, "y"_a, "size_x"_a, "size_y"_a, "materials"_a, "materials_stride"_a, "material_list"_a, "allocator"_a,
            "Set the material indices of a block of data.\n"
            "Beware this can create a race condition if you're running collision queries in parallel. See class documentation for more information.\n"
            "Args:\n"
            "    x (int): Start X position, must in the range [0, mSampleCount - 1].\n"
            "    y (int): Start Y position, must in the range [0, mSampleCount - 1].\n"
            "    size_x (int): Number of samples in X direction.\n"
            "    size_y (int): Number of samples in Y direction.\n"
            "    materials (uint8*): The new material indices, must be at least inSizeX * inSizeY uint8s. Values are returned in x-major order.\n"
            "    materials_stride (intptr_t): Stride in uint8s between two consecutive rows of inMaterials (can be negative if the data is upside down).\n"
            "    material_list (PhysicsMaterialList*): The material list to use for the new material indices or nullptr if the material list should not be updated.\n"
            "    allocator (TempAllocator): Allocator to use for temporary memory.\n"
            "Returns:\n"
            "    bool: True if the material indices were set, false if the total number of materials exceeded 256.")
        .def("save_binary_state", &HeightFieldShape::SaveBinaryState, "stream"_a)
        .def("save_material_state", &HeightFieldShape::SaveMaterialState, "materials"_a)
        .def("restore_material_state", &HeightFieldShape::RestoreMaterialState, "materials"_a, "num_materials"_a)
        .def("get_stats", &HeightFieldShape::GetStats)
        .def("get_volume", &HeightFieldShape::GetVolume)
        .def_static("register", &HeightFieldShape::sRegister)
        .def_rw_static("draw_triangle_outlines", &HeightFieldShape::sDrawTriangleOutlines);
}