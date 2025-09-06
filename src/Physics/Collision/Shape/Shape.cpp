#include "Common.h"
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>

#include <nanobind/stl/vector.h>

void BindShape(nb::module_ &m) {
    m.def("get_all_sub_shape_types", []() -> nb::typed<nb::list, nb::str> {
        nb::typed<nb::list, nb::str> out;
        for (size_t i = 0; i < _countof(sAllSubShapeTypes); ++i)
            out.append(nb::cast(sAllSubShapeTypes[i], nb::rv_policy::reference));
        return out;
    });

    m.def("get_convex_sub_shape_types", []() -> nb::typed<nb::list, nb::str> {
        nb::typed<nb::list, nb::str> out;
        for (size_t i = 0; i < _countof(sConvexSubShapeTypes); ++i)
            out.append(nb::cast(sConvexSubShapeTypes[i], nb::rv_policy::reference));
        return out;
    });

    m.def("get_compound_sub_shape_types", []() -> nb::typed<nb::list, nb::str> {
        nb::typed<nb::list, nb::str> out;
        for (size_t i = 0; i < _countof(sCompoundSubShapeTypes); ++i)
            out.append(nb::cast(sCompoundSubShapeTypes[i], nb::rv_policy::reference));
        return out;
    });

    m.def("get_decorator_sub_shape_types", []() -> nb::typed<nb::list, nb::str> {
        nb::typed<nb::list, nb::str> out;
        for (size_t i = 0; i < _countof(sDecoratorSubShapeTypes); ++i)
            out.append(nb::cast(sDecoratorSubShapeTypes[i], nb::rv_policy::reference));
        return out;
    });

    m.attr("NumSubShapeTypes") = NumSubShapeTypes;

    m.def("get_sub_shape_type_names", []() -> nb::typed<nb::list, nb::str> {
        nb::typed<nb::list, nb::str> out;
        for (size_t i = 0; i < NumSubShapeTypes; ++i)
            out.append(nb::cast(sSubShapeTypeNames[i], nb::rv_policy::reference));
        return out;
    });

    nb::enum_<EShapeType>(m, "EShapeType",
            "Shapes are categorized in groups, each shape can return which group it belongs to through its Shape::GetType function.")
        .value("CONVEX", EShapeType::Convex,
            "Used by ConvexShape, all shapes that use the generic convex vs convex collision detection system (box, sphere, capsule, tapered capsule, cylinder, triangle)")
        .value("COMPOUND", EShapeType::Compound,
            "Used by CompoundShape")
        .value("DECORATED", EShapeType::Decorated,
            "Used by DecoratedShape")
        .value("MESH", EShapeType::Mesh,
            "Used by MeshShape")
        .value("HEIGHT_FIELD", EShapeType::HeightField,
            "Used by HeightFieldShape")
        .value("SOFT_BODY", EShapeType::SoftBody,
            "Used by SoftBodyShape")
        .value("USER1", EShapeType::User1)
        .value("USER2", EShapeType::User2)
        .value("USER3", EShapeType::User3)
        .value("USER4", EShapeType::User4)
        .value("PLANE", EShapeType::Plane,
            "Used by PlaneShape")
        .value("EMPTY", EShapeType::Empty,
            "Used by EmptyShape");

    nb::enum_<EShapeSubType>(m, "EShapeSubType",
            "This enumerates all shape types, each shape can return its type through Shape::GetSubType")
        .value("SPHERE", EShapeSubType::Sphere)
        .value("BOX", EShapeSubType::Box)
        .value("TRIANGLE", EShapeSubType::Triangle)
        .value("CAPSULE", EShapeSubType::Capsule)
        .value("TAPERED_CAPSULE", EShapeSubType::TaperedCapsule)
        .value("CYLINDER", EShapeSubType::Cylinder)
        .value("CONVEX_HULL", EShapeSubType::ConvexHull)
        .value("STATIC_COMPOUND", EShapeSubType::StaticCompound)
        .value("MUTABLE_COMPOUND", EShapeSubType::MutableCompound)
        .value("ROTATED_TRANSLATED", EShapeSubType::RotatedTranslated)
        .value("SCALED", EShapeSubType::Scaled)
        .value("OFFSET_CENTER_OF_MASS", EShapeSubType::OffsetCenterOfMass)
        .value("MESH", EShapeSubType::Mesh)
        .value("HEIGHT_FIELD", EShapeSubType::HeightField)
        .value("SOFT_BODY", EShapeSubType::SoftBody)
        .value("USER1", EShapeSubType::User1)
        .value("USER2", EShapeSubType::User2)
        .value("USER3", EShapeSubType::User3)
        .value("USER4", EShapeSubType::User4)
        .value("USER5", EShapeSubType::User5)
        .value("USER6", EShapeSubType::User6)
        .value("USER7", EShapeSubType::User7)
        .value("USER8", EShapeSubType::User8)
        .value("USER_CONVEX1", EShapeSubType::UserConvex1)
        .value("USER_CONVEX2", EShapeSubType::UserConvex2)
        .value("USER_CONVEX3", EShapeSubType::UserConvex3)
        .value("USER_CONVEX4", EShapeSubType::UserConvex4)
        .value("USER_CONVEX5", EShapeSubType::UserConvex5)
        .value("USER_CONVEX6", EShapeSubType::UserConvex6)
        .value("USER_CONVEX7", EShapeSubType::UserConvex7)
        .value("USER_CONVEX8", EShapeSubType::UserConvex8)
        .value("PLANE", EShapeSubType::Plane)
        .value("TAPERED_CYLINDER", EShapeSubType::TaperedCylinder)
        .value("EMPTY", EShapeSubType::Empty);

    nb::class_<ShapeSettings, RefTarget<ShapeSettings>> shapeSettingsCls(m, "ShapeSettings",
        nb::intrusive_ptr<ShapeSettings>([](ShapeSettings *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Class that can construct shapes and that is serializable using the ObjectStream system.\n"
            "Can be used to store shape data in 'uncooked' form (i.e. in a form that is still human readable and authorable).\n"
            "Once the shape has been created using the Create() function, the data will be moved into the Shape class\n"
            "in a form that is optimized for collision detection. After this, the ShapeSettings object is no longer needed\n"
            "and can be destroyed. Each shape class has a derived class of the ShapeSettings object to store shape specific\n"
            "data.");
    shapeSettingsCls
        .def("create", &ShapeSettings::Create,
            "Create a shape according to the settings specified by this object.") // TODO:
        .def("clear_cached_result", &ShapeSettings::ClearCachedResult,
            "When creating a shape, the result is cached so that calling Create() again will return the same shape.\n"
            "If you make changes to the ShapeSettings you need to call this function to clear the cached result to allow Create() to build a new shape.")
        .def_rw("user_data", &ShapeSettings::mUserData,
            "User data (to be used freely by the application)");

    nb::class_<Shape, RefTarget<Shape>> shapeCls(m, "Shape",
        nb::intrusive_ptr<Shape>([](Shape *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), nb::is_weak_referenceable(), // Allow weak refrences, inherited by all subtypes
        "Base class for all shapes (collision volume of a body). Defines a virtual interface for collision detection.");
    shapeCls
        // .def(nb::init<EShapeType, EShapeSubType>(), "type"_a, "sub_type"_a,
        //     "Constructor")
        // .def(nb::init<EShapeType, EShapeSubType, const ShapeSettings &, ShapeSettings::ShapeResult &>(), "type"_a, "sub_type"_a, "settings"_a, "result"_a)
        .def("get_type", &Shape::GetType,
            "Get type")
        .def("get_sub_type", &Shape::GetSubType)
        .def("get_user_data", &Shape::GetUserData,
            "User data (to be used freely by the application)")
        .def("set_user_data", &Shape::SetUserData, "user_data"_a)
        .def("must_be_static", &Shape::MustBeStatic,
            "Check if this shape can only be used to create a static body or if it can also be dynamic/kinematic")
        .def("get_center_of_mass", &Shape::GetCenterOfMass,
            "All shapes are centered around their center of mass. This function returns the center of mass position that needs to be applied to transform the shape to where it was created.")
        .def("get_local_bounds", &Shape::GetLocalBounds,
            "Get local bounding box including convex radius, this box is centered around the center of mass rather than the world transform")
        .def("get_sub_shape_id_bits_recursive", &Shape::GetSubShapeIDBitsRecursive,
            "Get the max number of sub shape ID bits that are needed to be able to address any leaf shape in this shape. Used mainly for checking that it is smaller or equal than SubShapeID::MaxBits.")
        .def("get_world_space_bounds", nb::overload_cast<Mat44Arg, Vec3Arg>(&Shape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a,
            "Get world space bounds including convex radius.\n"
            "This shape is scaled by inScale in local space first.\n"
            "This function can be overridden to return a closer fitting world space bounding box, by default it will just transform what GetLocalBounds() returns.")
        .def("get_world_space_bounds", nb::overload_cast<DMat44Arg, Vec3Arg>(&Shape::GetWorldSpaceBounds, nb::const_), "center_of_mass_transform"_a, "scale"_a,
            "Get world space bounds including convex radius.")
        .def("get_inner_radius", &Shape::GetInnerRadius,
            "Returns the radius of the biggest sphere that fits entirely in the shape. In case this shape consists of multiple sub shapes, it returns the smallest sphere of the parts.\n"
            "This can be used as a measure of how far the shape can be moved without risking going through geometry.")
        .def("get_mass_properties", &Shape::GetMassProperties,
            "Calculate the mass and inertia of this shape")
        .def("get_leaf_shape", &Shape::GetLeafShape, "sub_shape_id"_a, "remainder"_a, nb::rv_policy::reference,
            "Get the leaf shape for a particular sub shape ID.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): The full sub shape ID that indicates the path to the leaf shape.\n"
            "    remainder (SubShapeID): What remains of the sub shape ID after removing the path to the leaf shape (could e.g. refer to a triangle within a MeshShape).\n"
            "Returns:\n"
            "    Shape*: The shape or null if the sub shape ID is invalid.")
        .def("get_material", &Shape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference,
            "Get the material assigned to a particular sub shape ID")
        .def("get_surface_normal", &Shape::GetSurfaceNormal, "sub_shape_id"_a, "local_surface_position"_a,
            "Get the surface normal of a particular sub shape ID and point on surface (all vectors are relative to center of mass for this shape).\n"
            "Note: When you have a CollideShapeResult or ShapeCastResult you should use -mPenetrationAxis.Normalized() as contact normal as GetSurfaceNormal will only return face normals (and not vertex or edge normals).")
        .def("get_supporting_face", &Shape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "scale"_a, "center_of_mass_transform"_a, "vertices"_a,
            "Get the vertices of the face that faces inDirection the most (includes any convex radius). Note that this function can only return faces of\n"
            "convex shapes or triangles, which is why a sub shape ID to get to that leaf must be provided.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): Sub shape ID of target shape.\n"
            "    direction (Vec3): Direction that the face should be facing (in local space to this shape).\n"
            "    center_of_mass_transform (Mat44): Transform to transform outVertices with.\n"
            "    scale (Vec3): Scale in local space of the shape (scales relative to its center of mass).\n"
            "    vertices (SupportingFace): Resulting face. The returned face can be empty if the shape doesn't have polygons to return (e.g. because it's a sphere). The face will be returned in world space.")
        .def("get_sub_shape_user_data", &Shape::GetSubShapeUserData, "sub_shape_id"_a,
            "Get the user data of a particular sub shape ID. Corresponds with the value stored in Shape::GetUserData of the leaf shape pointed to by inSubShapeID.")
        .def("get_sub_shape_transformed_shape",[](const Shape &self, const SubShapeID &id, const Vec3 &pos, const Quat &rot, const Vec3 &scale) {
            SubShapeID remainder;
            auto r = self.GetSubShapeTransformedShape(id, pos, rot, scale, remainder);
            return nb::make_tuple(r, remainder);
         }, "sub_shape_id"_a, "position_com"_a, "rotation"_a, "scale"_a,
            "Get the direct child sub shape and its transform for a sub shape ID.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): Sub shape ID that indicates the path to the leaf shape.\n"
            "    position_com (Vec3): The position of the center of mass of this shape.\n"
            "    rotation (Quat): The orientation of this shape.\n"
            "    scale (Vec3): Scale in local space of the shape (scales relative to its center of mass).\n"
            "    remainder (SubShapeID): The remainder of the sub shape ID after removing the sub shape.\n"
            "Returns:\n"
            "    TransformedShape: Direct child sub shape and its transform, note that the body ID and sub shape ID will be invalid.")
        .def("get_submerged_volume", &Shape::GetSubmergedVolume, "center_of_mass_transform"_a, "scale"_a, "surface"_a, "total_volume"_a, "submerged_volume"_a, "center_of_buoyancy"_a, "base_offset"_a,
            "Gets the properties needed to do buoyancy calculations for a body using this shape.\n"
            "Args:\n"
            "    center_of_mass_transform (Mat44): Transform that takes this shape (centered around center of mass) to world space (or a desired other space).\n"
            "    scale (Vec3): Scale in local space of the shape (scales relative to its center of mass).\n"
            "    surface (Plane): The surface plane of the liquid relative to inCenterOfMassTransform.\n"
            "    total_volume (float): On return this contains the total volume of the shape.\n"
            "    submerged_volume (float): On return this contains the submerged volume of the shape.\n"
            "    center_of_buoyancy (Vec3): On return this contains the world space center of mass of the submerged volume.\n"
            "    base_offset (Vec3): The offset to transform inCenterOfMassTransform to world space (in double precision mode this can be used to shift the whole operation closer to the origin). Only used for debug drawing.")
        .def("draw", &Shape::Draw, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "use_material_colors"_a, "draw_wireframe"_a,
            "Draw the shape at a particular location with a particular color (debugging purposes)")
        .def("draw_get_support_function", &Shape::DrawGetSupportFunction, "renderer"_a, "center_of_mass_transform"_a, "scale"_a, "color"_a, "draw_support_direction"_a,
            "Draw the results of the GetSupportFunction with the convex radius added back on to show any errors introduced by this process (only relevant for convex shapes)")
        .def("draw_get_supporting_face", &Shape::DrawGetSupportingFace, "renderer"_a, "center_of_mass_transform"_a, "scale"_a,
            "Draw the results of the GetSupportingFace function to show any errors introduced by this process (only relevant for convex shapes)")
        .def("cast_ray", nb::overload_cast<const RayCast &, const SubShapeIDCreator &, RayCastResult &>(&Shape::CastRay, nb::const_), "ray"_a, "sub_shape_id_creator"_a, "hit"_a,
            "Cast a ray against this shape, returns true if it finds a hit closer than ioHit.mFraction and updates that fraction. Otherwise ioHit is left untouched and the function returns false.\n"
            "Note that the ray should be relative to the center of mass of this shape (i.e. subtract Shape::GetCenterOfMass() from RayCast::mOrigin if you want to cast against the shape in the space it was created).\n"
            "Convex objects will be treated as solid (meaning if the ray starts inside, you'll get a hit fraction of 0) and back face hits against triangles are returned.\n"
            "If you want the surface normal of the hit use GetSurfaceNormal(ioHit.mSubShapeID2, inRay.GetPointOnRay(ioHit.mFraction)).")
        .def("cast_ray", nb::overload_cast<const RayCast &, const RayCastSettings &, const SubShapeIDCreator &, CastRayCollector &, const ShapeFilter &>(&Shape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a,
            "Cast a ray against this shape. Allows returning multiple hits through ioCollector. Note that this version is more flexible but also slightly slower than the CastRay function that returns only a single hit.\n"
            "If you want the surface normal of the hit use GetSurfaceNormal(collected sub shape ID, inRay.GetPointOnRay(collected faction)).")
        .def("collide_point", &Shape::CollidePoint, "point"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a,
            "Check if inPoint is inside this shape. For this tests all shapes are treated as if they were solid.\n"
            "Note that inPoint should be relative to the center of mass of this shape (i.e. subtract Shape::GetCenterOfMass() from inPoint if you want to test against the shape in the space it was created).\n"
            "For a mesh shape, this test will only provide sensible information if the mesh is a closed manifold.\n"
            "For each shape that collides, ioCollector will receive a hit.")
        .def("collide_soft_body_vertices", &Shape::CollideSoftBodyVertices, "center_of_mass_transform"_a, "scale"_a, "vertices"_a, "num_vertices"_a, "colliding_shape_index"_a,
            "Collides all vertices of a soft body with this shape and updates SoftBodyVertex::mCollisionPlane, SoftBodyVertex::mCollidingShapeIndex and SoftBodyVertex::mLargestPenetration if a collision with more penetration was found.\n"
            "Args:\n"
            "    center_of_mass_transform (Mat44): Center of mass transform for this shape relative to the vertices.\n"
            "    scale (Vec3): Scale in local space of the shape (scales relative to its center of mass).\n"
            "    vertices (CollideSoftBodyVertexIterator): The vertices of the soft body.\n"
            "    num_vertices (int): The number of vertices in inVertices.\n"
            "    colliding_shape_index (int): Value to store in CollideSoftBodyVertexIterator::mCollidingShapeIndex when a collision was found.")
        .def("collect_transformed_shapes", &Shape::CollectTransformedShapes, "box"_a, "position_com"_a, "rotation"_a, "scale"_a, "sub_shape_id_creator"_a, "collector"_a, "shape_filter"_a,
            "Collect the leaf transformed shapes of all leaf shapes of this shape.\n"
            "inBox is the world space axis aligned box which leaf shapes should collide with.\n"
            "inPositionCOM/inRotation/inScale describes the transform of this shape.\n"
            "inSubShapeIDCeator represents the current sub shape ID of this shape.")
        .def("transform_shape", &Shape::TransformShape, "center_of_mass_transform"_a, "collector"_a,
            "Transforms this shape and all of its children with inTransform, resulting shape(s) are passed to ioCollector.\n"
            "Note that not all shapes support all transforms (especially true for scaling), the resulting shape will try to match the transform as accurately as possible.\n"
            "Args:\n"
            "    center_of_mass_transform (Mat44): The transform (rotation, translation, scale) that the center of mass of the shape should get.\n"
            "    collector (TransformedShapeCollector): The transformed shapes will be passed to this collector.")
        .def("scale_shape", &Shape::ScaleShape, "scale"_a,
            "Scale this shape. Note that not all shapes support all scales, this will return a shape that matches the scale as accurately as possible. See Shape::IsValidScale for more information.\n"
            "Args:\n"
            "    scale (Vec3): The scale to use for this shape (note: this scale is applied to the entire shape in the space it was created, most other functions apply the scale in the space of the leaf shapes and from the center of mass!).")
        .def("get_triangles_start", &Shape::GetTrianglesStart, "context"_a, "box"_a, "position_com"_a, "rotation"_a, "scale"_a,
            "To start iterating over triangles, call this function first.\n"
            "ioContext is a temporary buffer and should remain untouched until the last call to GetTrianglesNext.\n"
            "inBox is the world space bounding in which you want to get the triangles.\n"
            "inPositionCOM/inRotation/inScale describes the transform of this shape.\n"
            "To get the actual triangles call GetTrianglesNext.")
        .def("get_triangles_next", [](const Shape &shape, Shape::GetTrianglesContext &context, int max_triangles) {
            std::vector<Float3> vertices(max_triangles * 3);
            std::vector<const PhysicsMaterial *> materials(max_triangles);

            int num_triangles = shape.GetTrianglesNext(context, max_triangles, vertices.data(), materials.data());

            if (num_triangles > 0) {
                vertices.resize(num_triangles * 3);
                materials.resize(num_triangles);

                return nb::make_tuple(vertices, materials);
            }

            return nb::make_tuple(std::vector<Float3>(), std::vector<const PhysicsMaterial *>());
         } , "context"_a, "max_triangles_requested"_a,
            "Call this repeatedly to get all triangles in the box.\n"
            "outTriangleVertices should be large enough to hold 3 * inMaxTriangleRequested entries.\n"
            "outMaterials (if it is not null) should contain inMaxTrianglesRequested entries.\n"
            "The function returns the amount of triangles that it found (which will be <= inMaxTrianglesRequested), or 0 if there are no more triangles.\n"
            "Note that the function can return a value < inMaxTrianglesRequested and still have more triangles to process (triangles can be returned in blocks).\n"
            "Note that the function may return triangles outside of the requested box, only coarse culling is performed on the returned triangles.\n"
            "Output [list[vertices], list[materials]]")
        .def("save_binary_state", &Shape::SaveBinaryState, "stream"_a,
            "Saves the contents of the shape in binary form to inStream.")
        .def_static("restore_from_binary_state", &Shape::sRestoreFromBinaryState, "stream"_a,
            "Creates a Shape of the correct type and restores its contents from the binary stream inStream.")
        .def("save_material_state", &Shape::SaveMaterialState, "materials"_a,
            "Outputs the material references that this shape has to outMaterials.")
        .def("restore_material_state", &Shape::RestoreMaterialState, "materials"_a, "num_materials"_a,
            "Restore the material references after calling sRestoreFromBinaryState. Note that the exact same materials need to be provided in the same order as returned by SaveMaterialState.")
        .def("save_sub_shape_state", &Shape::SaveSubShapeState, "sub_shapes"_a,
            "Outputs the shape references that this shape has to outSubShapes.")
        .def("restore_sub_shape_state", &Shape::RestoreSubShapeState, "sub_shapes"_a, "num_shapes"_a,
            "Restore the shape references after calling sRestoreFromBinaryState. Note that the exact same shapes need to be provided in the same order as returned by SaveSubShapeState.")
        .def("save_with_children", &Shape::SaveWithChildren, "stream"_a, "shape_map"_a, "material_map"_a,
            "Save this shape, all its children and its materials. Pass in an empty map in ioShapeMap / ioMaterialMap or reuse the same map while saving multiple shapes to the same stream in order to avoid writing duplicates.")
        .def_static("restore_with_children", &Shape::sRestoreWithChildren, "stream"_a, "shape_map"_a, "material_map"_a,
            "Restore a shape, all its children and materials. Pass in an empty map in ioShapeMap / ioMaterialMap or reuse the same map while reading multiple shapes from the same stream in order to restore duplicates.")
        .def("get_stats", &Shape::GetStats,
            "Get stats of this shape. Use for logging / data collection purposes only. Does not add values from child shapes, use GetStatsRecursive for this.")
        .def("get_stats_recursive", &Shape::GetStatsRecursive, "visited_shapes"_a,
            "Get the combined stats of this shape and its children.\n"
            "Args:\n"
            "    visited_shapes (VisitedShapes): is used to track which shapes have already been visited, to avoid calculating the wrong memory size.")
        .def("get_volume", &Shape::GetVolume,
            "Volume of this shape (m^3). Note that for compound shapes the volume may be incorrect since child shapes can overlap which is not accounted for.")
        .def("is_valid_scale", &Shape::IsValidScale, "scale"_a,
            "Test if inScale is a valid scale for this shape. Some shapes can only be scaled uniformly, compound shapes cannot handle shapes\n"
            "being rotated and scaled (this would cause shearing), scale can never be zero. When the scale is invalid, the function will return false.\n"
            "Here's a list of supported scales:\n"
            "* SphereShape: Scale must be uniform (signs of scale are ignored).\n"
            "* BoxShape: Any scale supported (signs of scale are ignored).\n"
            "* TriangleShape: Any scale supported when convex radius is zero, otherwise only uniform scale supported.\n"
            "* CapsuleShape: Scale must be uniform (signs of scale are ignored).\n"
            "* TaperedCapsuleShape: Scale must be uniform (sign of Y scale can be used to flip the capsule).\n"
            "* CylinderShape: Scale must be uniform in XZ plane, Y can scale independently (signs of scale are ignored).\n"
            "* RotatedTranslatedShape: Scale must not cause shear in the child shape.\n"
            "* CompoundShape: Scale must not cause shear in any of the child shapes.")
        .def("make_scale_valid", &Shape::MakeScaleValid, "scale"_a,
            "This function will make sure that if you wrap this shape in a ScaledShape that the scale is valid.\n"
            "Note that this involves discarding components of the scale that are invalid, so the resulting scaled shape may be different than the requested scale.\n"
            "Compare the return value of this function with the scale you passed in to detect major inconsistencies and possibly warn the user.\n"
            "Args:\n"
            "    scale (Vec3): Local space scale for this shape.\n"
            "Returns:\n"
            "    Vec3: Scale that can be used to wrap this shape in a ScaledShape. IsValidScale will return true for this scale.")
        .def_ro_static("C_GET_TRIANGLES_MIN_TRIANGLES_REQUESTED", &Shape::cGetTrianglesMinTrianglesRequested,
            "This is the minimum amount of triangles that should be requested through GetTrianglesNext.")
        .def_rw_static("draw_submerged_volumes", &Shape::sDrawSubmergedVolumes,
            "Debug helper which draws the intersection between water and the shapes, the center of buoyancy and the submerged volume");

    nb::class_<ShapeFunctions> shapeFunctionsCls(m, "ShapeFunctions",
            "Function table for functions on shapes");
    shapeFunctionsCls
        .def_static("get", &ShapeFunctions::sGet, "sub_type"_a,
            "Get an entry in the registry for a particular sub type")
        // .def_rw("construct", &ShapeFunctions::mConstruct,             // TODO:
            // "Construct a shape")
        .def_rw("color", &ShapeFunctions::mColor,
            "Color of the shape when drawing")
            ;

    nb::class_<Shape::GetTrianglesContext> getTrianglesContextCls(m, "GetTrianglesContext",
            "An opaque buffer that holds shape specific information during GetTrianglesStart/Next.");
    // getTrianglesContextCls
        // .def_rw("data", &Shape::GetTrianglesContext::mData); // TODO:

    nb::class_<Shape::Stats> statsCls(m, "Stats",
            "Class that holds information about the shape that can be used for logging / data collection purposes");
    statsCls
        .def(nb::init<size_t, uint>(), "size_bytes"_a, "num_triangles"_a)
        .def_rw("size_bytes", &Shape::Stats::mSizeBytes,
            "Amount of memory used by this shape (size in bytes)")
        .def_rw("num_triangles", &Shape::Stats::mNumTriangles,
            "Number of triangles in this shape (when applicable)");

}
