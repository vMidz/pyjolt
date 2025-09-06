
#include "Common.h"
#include <Jolt/Physics/Collision/TransformedShape.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/ShapeFilter.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <Jolt/Physics/Body/BodyID.h>
#include<Jolt/Physics/Collision/PhysicsMaterial.h>

void BindTransformedShape(nb::module_ &m) {
    nb::class_<TransformedShape> transformedShapeCls(m, "TransformedShape",
        "Temporary data structure that contains a shape and a transform.\n"
        "This structure can be obtained from a body (e.g. after a broad phase query) under lock protection.\n"
        "The lock can then be released and collision detection operations can be safely performed since\n"
        "the class takes a reference on the shape and does not use anything from the body anymore.");
    transformedShapeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<RVec3Arg, QuatArg, const Shape *, const BodyID &, const SubShapeIDCreator &>(), "position_com"_a, "rotation"_a, "shape"_a, "body_id"_a, "sub_shape_id_creator"_a)
        .def("cast_ray", nb::overload_cast<const RRayCast &, RayCastResult &>(&TransformedShape::CastRay, nb::const_), "ray"_a, "hit"_a,
            "Cast a ray and find the closest hit. Returns true if it finds a hit. Hits further than ioHit.mFraction will not be considered and in this case ioHit will remain unmodified (and the function will return false).\n"
            "Convex objects will be treated as solid (meaning if the ray starts inside, you'll get a hit fraction of 0) and back face hits are returned.\n"
            "If you want the surface normal of the hit use GetWorldSpaceSurfaceNormal(ioHit.mSubShapeID2, inRay.GetPointOnRay(ioHit.mFraction)) on this object.")
        .def("cast_ray", nb::overload_cast<const RRayCast &, const RayCastSettings &, CastRayCollector &, const ShapeFilter &>(&TransformedShape::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "collector"_a, "shape_filter"_a,
            "Cast a ray, allows collecting multiple hits. Note that this version is more flexible but also slightly slower than the CastRay function that returns only a single hit.\n"
            "If you want the surface normal of the hit use GetWorldSpaceSurfaceNormal(collected sub shape ID, inRay.GetPointOnRay(collected fraction)) on this object.")
        .def("collide_point", &TransformedShape::CollidePoint, "point"_a, "collector"_a, "shape_filter"_a,
            "Check if inPoint is inside any shapes. For this tests all shapes are treated as if they were solid.\n"
            "For a mesh shape, this test will only provide sensible information if the mesh is a closed manifold.\n"
            "For each shape that collides, ioCollector will receive a hit")
        .def("collide_shape", &TransformedShape::CollideShape, "shape"_a, "shape_scale"_a, "center_of_mass_transform"_a, "collide_shape_settings"_a, "base_offset"_a, "collector"_a, "shape_filter"_a,
            "Collide a shape and report any hits to ioCollector.\n"
            "Args:\n"
            "    shape (Shape*): Shape to test.\n"
            "    shape_scale (Vec3): Scale in local space of shape.\n"
            "    center_of_mass_transform (Mat44): Center of mass transform for the shape.\n"
            "    collide_shape_settings (CollideShapeSettings): Settings.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. mShapePositionCOM since floats are most accurate near the origin.\n"
            "    collector (CollideShapeCollector): Collector that receives the hits.\n"
            "    shape_filter (ShapeFilter): Filter that allows you to reject collisions.")
        .def("cast_shape", &TransformedShape::CastShape, "shape_cast"_a, "shape_cast_settings"_a, "base_offset"_a, "collector"_a, "shape_filter"_a,
            "Cast a shape and report any hits to ioCollector.\n"
            "Args:\n"
            "    shape_cast (RShapeCast): The shape cast and its position and direction.\n"
            "    shape_cast_settings (ShapeCastSettings): Settings for the shape cast.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. mShapePositionCOM or inShapeCast.mCenterOfMassStart.GetTranslation() since floats are most accurate near the origin.\n"
            "    collector (CastShapeCollector): Collector that receives the hits.\n"
            "    shape_filter (ShapeFilter): Filter that allows you to reject collisions.")
        .def("collect_transformed_shapes", &TransformedShape::CollectTransformedShapes, "box"_a, "collector"_a, "shape_filter"_a,
            "Collect the leaf transformed shapes of all leaf shapes of this shape\n"
            "inBox is the world space axis aligned box which leaf shapes should collide with")
        .def("get_triangles_start", &TransformedShape::GetTrianglesStart, "context"_a, "box"_a, "base_offset"_a,
            "To start iterating over triangles, call this function first.\n"
            "To get the actual triangles call GetTrianglesNext.\n"
            "Args:\n"
            "    context (GetTrianglesContext): A temporary buffer and should remain untouched until the last call to GetTrianglesNext.\n"
            "    box (AABox): The world space bounding in which you want to get the triangles.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. inBox.GetCenter() since floats are most accurate near the origin.")
        // .def("get_triangles_next", &TransformedShape::GetTrianglesNext, "context"_a, "max_triangles_requested"_a, "triangle_vertices"_a, "materials"_a = nullptr,
        //     "Call this repeatedly to get all triangles in the box.\n"
        //     "outTriangleVertices should be large enough to hold 3 * inMaxTriangleRequested entries\n"
        //     "outMaterials (if it is not null) should contain inMaxTrianglesRequested entries\n"
        //     "The function returns the amount of triangles that it found (which will be <= inMaxTrianglesRequested), or 0 if there are no more triangles.\n"
        //     "Note that the function can return a value < inMaxTrianglesRequested and still have more triangles to process (triangles can be returned in blocks)\n"
        //     "Note that the function may return triangles outside of the requested box, only coarse culling is performed on the returned triangles")
        .def("get_shape_scale", &TransformedShape::GetShapeScale,
            "Get/set the scale of the shape as a Vec3")
        .def("set_shape_scale", &TransformedShape::SetShapeScale, "scale"_a)
        .def("get_center_of_mass_transform", &TransformedShape::GetCenterOfMassTransform,
            "Calculates the transform for this shapes's center of mass (excluding scale)")
        .def("get_inverse_center_of_mass_transform", &TransformedShape::GetInverseCenterOfMassTransform,
            "Calculates the inverse of the transform for this shape's center of mass (excluding scale)")
        .def("set_world_transform", nb::overload_cast<RVec3Arg, QuatArg, Vec3Arg>(&TransformedShape::SetWorldTransform), "position"_a, "rotation"_a, "scale"_a,
            "Sets the world transform (including scale) of this transformed shape (not from the center of mass but in the space the shape was created)")
        .def("set_world_transform", nb::overload_cast<RMat44Arg>(&TransformedShape::SetWorldTransform), "transform"_a,
            "Sets the world transform (including scale) of this transformed shape (not from the center of mass but in the space the shape was created)")
        .def("get_world_transform", &TransformedShape::GetWorldTransform,
            "Calculates the world transform including scale of this shape (not from the center of mass but in the space the shape was created)")
        .def("get_world_space_bounds", &TransformedShape::GetWorldSpaceBounds,
            "Get the world space bounding box for this transformed shape")
        .def("make_sub_shape_id_relative_to_shape", &TransformedShape::MakeSubShapeIDRelativeToShape, "sub_shape_id"_a,
            "Make inSubShapeID relative to mShape. When mSubShapeIDCreator is not empty, this is needed in order to get the correct path to the sub shape.")
        .def("get_world_space_surface_normal", &TransformedShape::GetWorldSpaceSurfaceNormal, "sub_shape_id"_a, "position"_a,
            "Get surface normal of a particular sub shape and its world space surface position on this body.\n"
            "Note: When you have a CollideShapeResult or ShapeCastResult you should use -mPenetrationAxis.Normalized() as contact normal as GetWorldSpaceSurfaceNormal will only return face normals (and not vertex or edge normals).")
        .def("get_supporting_face", &TransformedShape::GetSupportingFace, "sub_shape_id"_a, "direction"_a, "base_offset"_a, "vertices"_a,
            "Get the vertices of the face that faces inDirection the most (includes any convex radius). Note that this function can only return faces of\n"
            "convex shapes or triangles, which is why a sub shape ID to get to that leaf must be provided.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): Sub shape ID of target shape.\n"
            "    direction (Vec3): Direction that the face should be facing (in world space).\n"
            "    base_offset (Vec3): The vertices will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. mShapePositionCOM since floats are most accurate near the origin.\n"
            "    vertices (Shape): Resulting face. Note the returned face can have a single point if the shape doesn't have polygons to return (e.g. because it's a sphere). The face will be returned in world space.")
        .def("get_material", &TransformedShape::GetMaterial, "sub_shape_id"_a, nb::rv_policy::reference,
            "Get material of a particular sub shape")
        .def("get_sub_shape_user_data", &TransformedShape::GetSubShapeUserData, "sub_shape_id"_a,
            "Get the user data of a particular sub shape")
        .def("get_sub_shape_transformed_shape", &TransformedShape::GetSubShapeTransformedShape, "sub_shape_id"_a, "remainder"_a,
            "Get the direct child sub shape and its transform for a sub shape ID.\n"
            "Args:\n"
            "    sub_shape_id (SubShapeID): Sub shape ID that indicates the path to the leaf shape.\n"
            "    remainder (SubShapeID): The remainder of the sub shape ID after removing the sub shape.\n"
            "Returns:\n"
            "    TransformedShape: Direct child sub shape and its transform, note that the body ID and sub shape ID will be invalid.")
        .def_static("get_body_id", &TransformedShape::sGetBodyID, "ts"_a,
            "Helper function to return the body id from a transformed shape. If the transformed shape is null an invalid body ID will be returned.")
        .def_rw("shape_position_com", &TransformedShape::mShapePositionCOM,
            "Center of mass world position of the shape")
        .def_rw("shape_rotation", &TransformedShape::mShapeRotation,
            "Rotation of the shape")
        .def_rw("shape", &TransformedShape::mShape,
            "The shape itself")
        .def_rw("shape_scale", &TransformedShape::mShapeScale,
            "Not stored as Vec3 to get a nicely packed structure")
        .def_rw("body_id", &TransformedShape::mBodyID,
            "Optional body ID from which this shape comes")
        .def_rw("sub_shape_id_creator", &TransformedShape::mSubShapeIDCreator,
            "Optional sub shape ID creator for the shape (can be used when expanding compound shapes into multiple transformed shapes)");
}