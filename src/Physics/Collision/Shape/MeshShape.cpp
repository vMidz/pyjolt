#include "Common.h"
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

void BindMeshShape(nb::module_ &m) {
    nb::class_<MeshShapeSettings, ShapeSettings> meshShapeSettingsCls(m, "MeshShapeSettings",
        "Class that constructs a MeshShape",
        nb::is_final());
    meshShapeSettingsCls
        .def(nb::init<>(),
            "Default constructor for deserialization")
        .def(nb::init<const TriangleList &, PhysicsMaterialList>(), "triangles"_a, "materials"_a = PhysicsMaterialList (),
            "Create a mesh shape.")
        .def(nb::init<VertexList, IndexedTriangleList, PhysicsMaterialList>(), "vertices"_a, "triangles"_a, "materials"_a = PhysicsMaterialList ())
        .def("sanitize", &MeshShapeSettings::Sanitize,
            "Sanitize the mesh data. Remove duplicate and degenerate triangles. This is called automatically when constructing the MeshShapeSettings with a list of (indexed-) triangles.")
        .def("create", &MeshShapeSettings::Create)
        .def_rw("triangle_vertices", &MeshShapeSettings::mTriangleVertices,
            "Vertices belonging to mIndexedTriangles")
        .def_rw("indexed_triangles", &MeshShapeSettings::mIndexedTriangles,
            "Original list of indexed triangles (triangles will be reordered internally in the mesh shape).\n"
            "Triangles must be provided in counter clockwise order.\n"
            "Degenerate triangles will automatically be removed during mesh creation but no other mesh simplifications are performed, use an external library if this is desired.\n"
            "For simulation, the triangles are considered to be single sided.\n"
            "For ray casts you can choose to make triangles double sided by setting RayCastSettings::mBackFaceMode to EBackFaceMode::CollideWithBackFaces.\n"
            "For collide shape tests you can use CollideShapeSettings::mBackFaceMode and for shape casts you can use ShapeCastSettings::mBackFaceModeTriangles.")
        .def_rw("materials", &MeshShapeSettings::mMaterials,
            "Materials assigned to the triangles. Each triangle specifies which material it uses through its mMaterialIndex")
        .def_rw("max_triangles_per_leaf", &MeshShapeSettings::mMaxTrianglesPerLeaf,
            "Maximum number of triangles in each leaf of the axis aligned box tree. This is a balance between memory and performance. Can be in the range [1, MeshShape::MaxTrianglesPerLeaf].\n"
            "Sensible values are between 4 (for better performance) and 8 (for less memory usage).")
        .def_rw("active_edge_cos_threshold_angle", &MeshShapeSettings::mActiveEdgeCosThresholdAngle,
            "Cosine of the threshold angle (if the angle between the two triangles is bigger than this, the edge is active, note that a concave edge is always inactive).\n"
            "Setting this value too small can cause ghost collisions with edges, setting it too big can cause depenetration artifacts (objects not depenetrating quickly).\n"
            "Valid ranges are between cos(0 degrees) and cos(90 degrees). The default value is cos(5 degrees).")
        .def_rw("per_triangle_user_data", &MeshShapeSettings::mPerTriangleUserData,
            "When true, we store the user data coming from Triangle::mUserData or IndexedTriangle::mUserData in the mesh shape.\n"
            "This can be used to store additional data like the original index of the triangle in the mesh.\n"
            "Can be retrieved using MeshShape::GetTriangleUserData.\n"
            "Turning this on increases the memory used by the MeshShape by roughly 25%.")
        .def("__repr__", [](const MeshShapeSettings &self) {
            return nb::str("MeshShapeSettings(num_vertices={}, num_indexed_triangles={}, num_materials={}, max_leaf_tris={}, per_tri_ud={})")
                .format(self.mTriangleVertices.size(), self.mIndexedTriangles.size(), self.mMaterials.size(), self.mMaxTrianglesPerLeaf, self.mPerTriangleUserData);
        });

    nb::class_<MeshShape, Shape> meshShapeCls(m, "MeshShape",
        "A mesh shape, consisting of triangles. Mesh shapes are mostly used for static geometry.\n"
        "They can be used by dynamic or kinematic objects but only if they don't collide with other mesh or heightfield shapes as those collisions are currently not supported.\n"
        "Note that if you make a mesh shape a dynamic or kinematic object, you need to provide a mass yourself as mesh shapes don't need to form a closed hull so don't have a well defined volume from which the mass can be calculated.",
        nb::is_final());
    meshShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<const MeshShapeSettings &, ShapeSettings::ShapeResult &>(), "settings"_a, "result"_a)
        .def("must_be_static", &MeshShape::MustBeStatic)
        .def("get_local_bounds", &MeshShape::GetLocalBounds)
        .def("get_sub_shape_id_bits_recursive", &MeshShape::GetSubShapeIDBitsRecursive)
        .def("get_inner_radius", &MeshShape::GetInnerRadius)
        .def("get_mass_properties", &MeshShape::GetMassProperties)
        .def("get_material", &MeshShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference)
        .def("get_material_list", &MeshShape::GetMaterialList,
            "Get the list of all materials")
        .def("get_material_index", &MeshShape::GetMaterialIndex, "sub_shape_id"_a,
            "Determine which material index a particular sub shape uses (note that if there are no materials this function will return 0 so check the array size)\n"
            "Note: This could for example be used to create a decorator shape around a mesh shape that overrides the GetMaterial call to replace a material with another material.")
        .def("get_surface_normal", &MeshShape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a)
        .def("get_supporting_face", [](const MeshShape &self, const SubShapeID &inSubShapeID,
            Vec3Arg inDirection, Vec3Arg inScale, Mat44Arg inCenterOfMassTransform) {

            using SupportingFace = StaticArray<Vec3, 32>;
            SupportingFace outVertices;
            self.GetSupportingFace(inSubShapeID, inDirection, inScale, inCenterOfMassTransform, outVertices);
            return outVertices;
        }, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a)
        .def("draw", &MeshShape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&MeshShape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a)
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&MeshShape::CastRay, nb::const_),
            "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a)
        .def("collide_point", &MeshShape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a,
            "See: Shape::CollidePoint\n"
            "Note that for CollidePoint to work for a mesh shape, the mesh needs to be closed (a manifold) or multiple non-intersecting manifolds. Triangles may be facing the interior of the manifold.\n"
            "Insideness is tested by counting the amount of triangles encountered when casting an infinite ray from inPoint. If the number of hits is odd we're inside, if it's even we're outside.")
        .def("collide_soft_body_vertices", &MeshShape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a)
        .def("get_triangles_start", &MeshShape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a)
        // .def("get_triangles_next", &MeshShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr)
        .def("get_submerged_volume", [](MeshShape &self, Mat44Arg inCenterOfMassTransform, Vec3Arg inScale, const Plane &inSurface, RVec3Arg inBaseOffset){
            float outTotalVolume;
            float outSubmergedVolume;
            Vec3 outCenterOfBuoyancy;
            self.GetSubmergedVolume(inCenterOfMassTransform, inScale, inSurface, outTotalVolume,
                outSubmergedVolume, outCenterOfBuoyancy, inBaseOffset);
            return nb::make_tuple(outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy);
        }, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "base_offset"_a,
           "Output: [outTotalVolume, outSubmergedVolume, outCenterOfBuoyancy]")
        .def("save_binary_state", &MeshShape::SaveBinaryState, "stream"_a)
        .def("save_material_state", &MeshShape::SaveMaterialState, "materials"_a)
        .def("restore_material_state", &MeshShape::RestoreMaterialState, "materials"_a, "num_materials"_a)
        .def("get_stats", &MeshShape::GetStats)
        .def("get_volume", &MeshShape::GetVolume)
        .def("get_triangle_user_data", &MeshShape::GetTriangleUserData, "sub_shape_id"_a)
        .def_static("register", &MeshShape::sRegister)
        .def_rw_static("draw_triangle_groups", &MeshShape::sDrawTriangleGroups)
        .def_rw_static("draw_triangle_outlines", &MeshShape::sDrawTriangleOutlines)
        .def("__repr__", [](const MeshShape &self) {
            Shape::Stats stats = self.GetStats();
            return nb::str("MeshShape(num_materials={}, memory_size={})")
                .format(self.GetMaterialList().size(), stats.mSizeBytes);
        });
}