#include "Common.h"
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

template <class CollectorType>
static void BindAllHitCollisionCollector(nb::module_ &m, const char *name) {
    using CurrentInst = AllHitCollisionCollector<CollectorType>;
    nb::class_<CurrentInst, CollectorType> allHitCollisionCollectorCls(m, name,
        "Simple implementation that collects all hits and optionally sorts them on distance");
    allHitCollisionCollectorCls
        .def(nb::init<>())
        .def("reset", &CurrentInst::Reset)
        .def("add_hit", &CurrentInst::AddHit, "result"_a)
        .def("sort", &CurrentInst::Sort,
            "Order hits on closest first")
        .def("had_hit", &CurrentInst::HadHit,
            "Check if any hits were collected")
        .def_rw("hits", &CurrentInst::mHits);
}

template <class CollectorType>
static void BindClosestHitCollisionCollector(nb::module_ &m, const char *name) {
    using CurrentInst = ClosestHitCollisionCollector<CollectorType>;
    nb::class_<CurrentInst, CollectorType> closestHitCollisionCollectorCls(m, name,
        "Simple implementation that collects the closest / deepest hit");
    closestHitCollisionCollectorCls
        .def(nb::init<>())
        .def("reset", &CurrentInst::Reset)
        .def("add_hit", &CurrentInst::AddHit, "result"_a)
        .def("had_hit", &CurrentInst::HadHit,
            "Check if this collector has had a hit")
        .def_rw("hit", &CurrentInst::mHit);
}

template <class CollectorType>
static void BindAnyHitCollisionCollector(nb::module_ &m, const char *name) {
    using CurrentInst = AnyHitCollisionCollector<CollectorType>;
    nb::class_<CurrentInst, CollectorType> anyHitCollisionCollectorCls(m, name,
        "Simple implementation that collects any hit");
    anyHitCollisionCollectorCls
        .def(nb::init<>())
        .def("reset", &CurrentInst::Reset)
        .def("add_hit", &CurrentInst::AddHit, "result"_a)
        .def("had_hit", &CurrentInst::HadHit,
            "Check if this collector has had a hit")
        .def_rw("hit", &CurrentInst::mHit);
}

void BindCollisionCollectorImpl(nb::module_ &m) {
    BindAllHitCollisionCollector<CastShapeCollector>(m, "AllHitCollisionCollector_CastShapeCollector");

    BindClosestHitCollisionCollector<CastShapeCollector>(m, "ClosestHitCollisionCollector_CastShapeCollector");

    BindAnyHitCollisionCollector<CastShapeCollector>(m, "AnyHitCollisionCollector_CastShapeCollector");
}