#include "Common.h"
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

void BindNarrowPhaseQuery(nb::module_ &m) {
    nb::class_<NarrowPhaseQuery, NonCopyable> narrowPhaseQueryCls(m, "NarrowPhaseQuery",
        "Class that provides an interface for doing precise collision detection against the broad and then the narrow phase.\n"
        "Unlike a BroadPhaseQuery, the NarrowPhaseQuery will test against shapes and will return collision information against triangles, spheres etc.");
    narrowPhaseQueryCls
        .def("init", &NarrowPhaseQuery::Init, "body_lock_interface"_a, "broad_phase_query"_a,
            "Initialize the interface (should only be called by PhysicsSystem)")
        .def("cast_ray", nb::overload_cast<const RRayCast &, RayCastResult &, const BroadPhaseLayerFilter &, const ObjectLayerFilter &, const BodyFilter &>(&NarrowPhaseQuery::CastRay, nb::const_), "ray"_a, "hit"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a,
            "Cast a ray and find the closest hit. Returns true if it finds a hit. Hits further than ioHit.mFraction will not be considered and in this case ioHit will remain unmodified (and the function will return false).\n"
            "Convex objects will be treated as solid (meaning if the ray starts inside, you'll get a hit fraction of 0) and back face hits against triangles are returned.\n"
            "If you want the surface normal of the hit use Body::GetWorldSpaceSurfaceNormal(ioHit.mSubShapeID2, inRay.GetPointOnRay(ioHit.mFraction)) on body with ID ioHit.mBodyID.")
        .def("cast_ray", nb::overload_cast<const RRayCast &, const RayCastSettings &, CastRayCollector &, const BroadPhaseLayerFilter &, const ObjectLayerFilter &, const BodyFilter &, const ShapeFilter &>(&NarrowPhaseQuery::CastRay, nb::const_), "ray"_a, "ray_cast_settings"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a,
            "Cast a ray, allows collecting multiple hits. Note that this version is more flexible but also slightly slower than the CastRay function that returns only a single hit.\n"
            "If you want the surface normal of the hit use Body::GetWorldSpaceSurfaceNormal(collected sub shape ID, inRay.GetPointOnRay(collected fraction)) on body with collected body ID.")
        .def("collide_point", &NarrowPhaseQuery::CollidePoint, "point"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a,
            "Check if inPoint is inside any shapes. For this tests all shapes are treated as if they were solid.\n"
            "For a mesh shape, this test will only provide sensible information if the mesh is a closed manifold.\n"
            "For each shape that collides, ioCollector will receive a hit")
        .def("collide_shape", &NarrowPhaseQuery::CollideShape, "shape"_a, "shape_scale"_a, "center_of_mass_transform"_a, "collide_shape_settings"_a, "base_offset"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a,
            "Collide a shape with the system.\n"
            "Args:\n"
            "    shape (Shape*): Shape to test.\n"
            "    shape_scale (Vec3): Scale in local space of shape.\n"
            "    center_of_mass_transform (Mat44): Center of mass transform for the shape.\n"
            "    collide_shape_settings (CollideShapeSettings): Settings.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. inCenterOfMassTransform.GetTranslation() since floats are most accurate near the origin.\n"
            "    collector (CollideShapeCollector): Collector that receives the hits.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that filters at broadphase level.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that filters at layer level.\n"
            "    body_filter (BodyFilter): Filter that filters at body level.\n"
            "    shape_filter (ShapeFilter): Filter that filters at shape level.")
        .def("cast_shape", &NarrowPhaseQuery::CastShape, "shape_cast"_a, "shape_cast_settings"_a, "base_offset"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a,
            "Cast a shape and report any hits to ioCollector.\n"
            "Args:\n"
            "    shape_cast (RShapeCast): The shape cast and its position and direction.\n"
            "    shape_cast_settings (ShapeCastSettings): Settings for the shape cast.\n"
            "    base_offset (Vec3): All hit results will be returned relative to this offset, can be zero to get results in world position, but when you're testing far from the origin you get better precision by picking a position that's closer e.g. inShapeCast.mCenterOfMassStart.GetTranslation() since floats are most accurate near the origin.\n"
            "    collector (CastShapeCollector): Collector that receives the hits.\n"
            "    broad_phase_layer_filter (BroadPhaseLayerFilter): Filter that filters at broadphase level.\n"
            "    object_layer_filter (ObjectLayerFilter): Filter that filters at layer level.\n"
            "    body_filter (BodyFilter): Filter that filters at body level.\n"
            "    shape_filter (ShapeFilter): Filter that filters at shape level.")
        .def("collect_transformed_shapes", &NarrowPhaseQuery::CollectTransformedShapes, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a, "body_filter"_a, "shape_filter"_a,
            "Collect all leaf transformed shapes that fall inside world space box inBox");
}