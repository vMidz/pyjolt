#include "Common.h"
#include <nanobind/trampoline.h>
#include <nanobind/stl/string.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Core/NonCopyable.h>

using ObjectLayerType = uint16;

class PyObjectLayerFilter : public ObjectLayerFilter {
  public:
    NB_TRAMPOLINE(ObjectLayerFilter, 1);
    ~PyObjectLayerFilter() override {
    }

    bool ShouldCollide(ObjectLayerType inLayer) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer);
    }

    // JPH_TRACK_BROADPHASE_STATS not defined
    // String GetDescription() const override {
    //     NB_OVERRIDE_NAME(
    //         "get_description",
    //         GetDescription
    //     );
    // }
};

class PyObjectLayerPairFilter : public ObjectLayerPairFilter {
  public:
    NB_TRAMPOLINE(ObjectLayerPairFilter, 1);

    ~PyObjectLayerPairFilter() override {
    }

    bool ShouldCollide(ObjectLayerType inLayer1, ObjectLayerType inLayer2) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer1,
            inLayer2);
    }
};

class PyDefaultObjectLayerFilter : public DefaultObjectLayerFilter {
  public:
    NB_TRAMPOLINE(DefaultObjectLayerFilter, 1);

    ~PyDefaultObjectLayerFilter() override {
    }

    bool ShouldCollide(ObjectLayer inLayer) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer);
    }
};

class PySpecifiedObjectLayerFilter : public SpecifiedObjectLayerFilter {
  public:
    NB_TRAMPOLINE(SpecifiedObjectLayerFilter, 1);

    ~PySpecifiedObjectLayerFilter() override {
    }

    bool ShouldCollide(ObjectLayer inLayer) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer);
    }
};

void BindObjectLayer(nb::module_ &m) {
    m.attr("OBJECT_LAYER_INVALID") = cObjectLayerInvalid;

    nb::class_<ObjectLayerFilter, PyObjectLayerFilter>(m, "ObjectLayerFilter",
        "Filter class for object layers")
        .def(nb::init<>())
        .def("should_collide", &ObjectLayerFilter::ShouldCollide, "layer"_a,
            "Function to filter out object layers when doing collision query test (return true to allow testing against objects with this layer)")
        // .def("get_description", &ObjectLayerFilter::GetDescription,
            // "Get a string that describes this filter for stat tracking purposes. Default is 'No Description'.")
        ;

    nb::class_<ObjectLayerPairFilter, PyObjectLayerPairFilter>(m, "ObjectLayerPairFilter",
        "Filter class to test if two objects can collide based on their object layer.")
        .def(nb::init<>())
        .def("should_collide", &ObjectLayerPairFilter::ShouldCollide,
            "layer1"_a, "layer2"_a, "Returns true if two layers can collide");

    nb::class_<DefaultObjectLayerFilter, PyDefaultObjectLayerFilter>(m, "DefaultObjectLayerFilter",
        "Default filter class that uses the pair filter in combination with a specified layer to filter layers.")
        .def(nb::init<const ObjectLayerPairFilter &, ObjectLayer>(), "Constructor")
        .def("should_collide", &DefaultObjectLayerFilter::ShouldCollide, "layer"_a)
        ;

    nb::class_<SpecifiedObjectLayerFilter, PySpecifiedObjectLayerFilter>(m, "SpecifiedObjectLayerFilter", "Allows objects from a specific layer only")
        .def(nb::init<ObjectLayerType>(), "layer"_a, "Constructor.")
        .def("should_collide", &SpecifiedObjectLayerFilter::ShouldCollide, "layer"_a)
        ;
}
