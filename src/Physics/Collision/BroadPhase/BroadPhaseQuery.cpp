#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Core/NonCopyable.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Geometry/AABox4.h>
#include <Jolt/Physics/Collision/AABoxCast.h>
#include <Jolt/Physics/Collision/RayCast.h>

void BindBroadPhaseQuery(nb::module_ &m) {
    nb::class_<BroadPhaseQuery, NonCopyable> broadPhaseQueryCls(m, "BroadPhaseQuery",
        "Interface to the broadphase that can perform collision queries. These queries will only test the bounding box of the body to quickly determine a potential set of colliding bodies.\n"
        "The shapes of the bodies are not tested, if you want this then you should use the NarrowPhaseQuery interface.");

        // TODO: default values = bad cast
    broadPhaseQueryCls
        .def("cast_ray", &BroadPhaseQuery::CastRay, "ray"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Cast a ray and add any hits to ioCollector")
        .def("collide_aa_box", &BroadPhaseQuery::CollideAABox, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Get bodies intersecting with inBox and any hits to ioCollector")
        .def("collide_sphere", &BroadPhaseQuery::CollideSphere, "center"_a, "radius"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Get bodies intersecting with a sphere and any hits to ioCollector")
        .def("collide_point", &BroadPhaseQuery::CollidePoint, "point"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Get bodies intersecting with a point and any hits to ioCollector")
        .def("collide_oriented_box", &BroadPhaseQuery::CollideOrientedBox, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Get bodies intersecting with an oriented box and any hits to ioCollector")
        .def("cast_aa_box", &BroadPhaseQuery::CastAABox, "box"_a, "collector"_a, "broad_phase_layer_filter"_a, "object_layer_filter"_a,
            "Cast a box and add any hits to ioCollector");
}