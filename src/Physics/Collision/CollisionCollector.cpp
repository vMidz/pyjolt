#include "Common.h"
#include <Jolt/Physics/Collision/CollisionCollector.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/CollidePointResult.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include<Jolt/Physics/Body/BodyPair.h>

#include <nanobind/trampoline.h>

template <class ResultTypeArg, class TraitsType>
class PyCollisionCollectorCastRay : public CollisionCollector<ResultTypeArg, TraitsType> {
  public:
    using TargetCollectorType = CollisionCollector<ResultTypeArg, TraitsType>;

    NB_TRAMPOLINE(TargetCollectorType, 4);

    ~PyCollisionCollectorCastRay() override = default;

    void Reset() override {
        NB_OVERRIDE_NAME(
            "reset",
            Reset);
    }

    void OnBody(const Body &inBody) override {
        NB_OVERRIDE_NAME(
            "on_body",
            OnBody,
            inBody);
    }

    void SetUserData(uint64 inUserData) override {
        NB_OVERRIDE_NAME(
            "set_user_data",
            SetUserData,
            inUserData);
    }

    void AddHit(const ResultTypeArg &inResult) override {
        NB_OVERRIDE_PURE_NAME(
            "add_hit",
            AddHit,
            inResult);
    }
};

template <class ResultTypeArg, class TraitsType>
static void BindInst(nb::module_ &m, const char *className) {
    using CurrentInst = CollisionCollector<ResultTypeArg, TraitsType>;

    nb::class_<CurrentInst, PyCollisionCollectorCastRay<ResultTypeArg, TraitsType>> collisionCollectorCls(m, className,
        "Virtual interface that allows collecting multiple collision results");
    collisionCollectorCls
        .def(nb::init<>(),
            "Default constructor")
        // .def(nb::init<>(), "rhs"_a,                             // TODO: template ctor
            // "Constructor to initialize from another collector")
        // .def(nb::init<const CurrentInst &>(), "rhs"_a)
        .def("reset", &CurrentInst::Reset,
            "If you want to reuse this collector, call Reset()")
        .def("on_body", &CurrentInst::OnBody, "body"_a,
            "When running a query through the NarrowPhaseQuery class, this will be called for every body that is potentially colliding.\n"
            "It allows collecting additional information needed by the collision collector implementation from the body under lock protection\n"
            "before AddHit is called (e.g. the user data pointer or the velocity of the body).")
        .def("set_context", &CurrentInst::SetContext, "context"_a,
            "Set by the collision detection functions to the current TransformedShape that we're colliding against before calling the AddHit function")
        .def("get_context", &CurrentInst::GetContext, nb::rv_policy::reference)
        .def("set_user_data", &CurrentInst::SetUserData, "user_data"_a,
            "This function can be used to set some user data on the collision collector")
        .def("add_hit", &CurrentInst::AddHit, "result"_a,
            "This function will be called for every hit found, it's up to the application to decide how to store the hit")
        .def("update_early_out_fraction", &CurrentInst::UpdateEarlyOutFraction, "fraction"_a,
            "Update the early out fraction (should be lower than before)")
        .def("reset_early_out_fraction", &CurrentInst::ResetEarlyOutFraction, "fraction"_a = TraitsType::InitialEarlyOutFraction,
            "Reset the early out fraction to a specific value")
        .def("force_early_out", &CurrentInst::ForceEarlyOut,
            "Force the collision detection algorithm to terminate as soon as possible. Call this from the AddHit function when a satisfying hit is found.")
        .def("should_early_out", &CurrentInst::ShouldEarlyOut,
            "When true, the collector will no longer accept any additional hits and the collision detection routine should early out as soon as possible")
        .def("get_early_out_fraction", &CurrentInst::GetEarlyOutFraction,
            "Get the current early out value")
        .def("get_positive_early_out_fraction", &CurrentInst::GetPositiveEarlyOutFraction,
            "Get the current early out value but make sure it's bigger than zero, this is used for shape casting as negative values are used for penetration")
        ;
}

void BindCollisionCollector(nb::module_ &m) {
    nb::class_<CollisionCollectorTraitsCastRay> collisionCollectorTraitsCastRayCls(m, "CollisionCollectorTraitsCastRay",
        "Traits to use for CastRay");
    collisionCollectorTraitsCastRayCls
        .def_ro_static("INITIAL_EARLY_OUT_FRACTION", &CollisionCollectorTraitsCastRay::InitialEarlyOutFraction,
            "For rays the early out fraction is the fraction along the line to order hits.")
        .def_ro_static("SHOULD_EARLY_OUT_FRACTION", &CollisionCollectorTraitsCastRay::ShouldEarlyOutFraction,
            "Furthest hit: Fraction is 1 + epsilon");

    nb::class_<CollisionCollectorTraitsCastShape> collisionCollectorTraitsCastShapeCls(m, "CollisionCollectorTraitsCastShape",
        "Traits to use for CastShape");
    collisionCollectorTraitsCastShapeCls
        .def_ro_static("INITIAL_EARLY_OUT_FRACTION", &CollisionCollectorTraitsCastShape::InitialEarlyOutFraction,
            "For rays the early out fraction is the fraction along the line to order hits.")
        .def_ro_static("SHOULD_EARLY_OUT_FRACTION", &CollisionCollectorTraitsCastShape::ShouldEarlyOutFraction,
            "Furthest hit: Fraction is 1 + epsilon");

    nb::class_<CollisionCollectorTraitsCollideShape> collisionCollectorTraitsCollideShapeCls(m, "CollisionCollectorTraitsCollideShape",
        "Traits to use for CollideShape");
    collisionCollectorTraitsCollideShapeCls
        .def_ro_static("INITIAL_EARLY_OUT_FRACTION", &CollisionCollectorTraitsCollideShape::InitialEarlyOutFraction,
            "For shape collisions we use -penetration depth to order hits.")
        .def_ro_static("SHOULD_EARLY_OUT_FRACTION", &CollisionCollectorTraitsCollideShape::ShouldEarlyOutFraction,
            "Most shallow hit: Separation is infinite");

    // TODO: ugly names
    BindInst<BroadPhaseCastResult, CollisionCollectorTraitsCastRay>(m, "BroadPhaseCastResult_CollisionCollectorTraitsCastRay");
    BindInst<RayCastResult, CollisionCollectorTraitsCastRay>(m, "RayCastResult_CollisionCollectorTraitsCastRay");

    BindInst<BroadPhaseCastResult, CollisionCollectorTraitsCastShape>(m, "BroadPhaseCastResult_CollisionCollectorTraitsCastShape");
    BindInst<ShapeCastResult, CollisionCollectorTraitsCastShape>(m, "ShapeCastResult_CollisionCollectorTraitsCastShape");

    BindInst<BodyID, CollisionCollectorTraitsCollideShape>(m, "BodyID_CollisionCollectorTraitsCollideShape");
    BindInst<CollidePointResult, CollisionCollectorTraitsCollideShape>(m, "CollidePointResult_CollisionCollectorTraitsCollideShape");
    BindInst<CollideShapeResult, CollisionCollectorTraitsCollideShape>(m, "CollideShapeResult_CollisionCollectorTraitsCollideShape");
    BindInst<TransformedShape, CollisionCollectorTraitsCollideShape>(m, "TransformedShape_CollisionCollectorTraitsCollideShape");

    BindInst<BodyPair, CollisionCollectorTraitsCollideShape>(m, "BodyPair_CollisionCollectorTraitsCollideShape");
}