#include "Common.h"
#include <nanobind/operators.h>
#include <nanobind/trampoline.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include <nanobind/stl/string.h>

class PyBroadPhaseLayerInterface : public BroadPhaseLayerInterface {
  public:
    NB_TRAMPOLINE(BroadPhaseLayerInterface, 5);

    ~PyBroadPhaseLayerInterface() override {
    }

    uint GetNumBroadPhaseLayers() const override {
        NB_OVERRIDE_PURE_NAME(
            "get_num_broad_phase_layers",
            GetNumBroadPhaseLayers, );
    }

    BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
        NB_OVERRIDE_PURE_NAME(
            "get_broad_phase_layer",
            GetBroadPhaseLayer,
            inLayer);
    }

    const char *GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
        NB_OVERRIDE_PURE_NAME(
            "get_broad_phase_layer_name",
            GetBroadPhaseLayerName,
            inLayer);
    }
};

class PyObjectVsBroadPhaseLayerFilter : public ObjectVsBroadPhaseLayerFilter {
  public:
    NB_TRAMPOLINE(ObjectVsBroadPhaseLayerFilter, 1);

    ~PyObjectVsBroadPhaseLayerFilter() override {
    }

    bool ShouldCollide([[maybe_unused]] ObjectLayer inLayer1, [[maybe_unused]] BroadPhaseLayer inLayer2) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer1,
            inLayer2);
    }
};

class PyBroadPhaseLayerFilter : public BroadPhaseLayerFilter {
  public:
    NB_TRAMPOLINE(BroadPhaseLayerFilter, 1);
    ~PyBroadPhaseLayerFilter() {
    }
    bool ShouldCollide(BroadPhaseLayer inLayer) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer);
    }
};

class PyDefaultBroadPhaseLayerFilter : public DefaultBroadPhaseLayerFilter {
  public:
    NB_TRAMPOLINE(DefaultBroadPhaseLayerFilter, 1);

    PyDefaultBroadPhaseLayerFilter(const ObjectVsBroadPhaseLayerFilter &inObjectVsBroadPhaseLayerFilter, ObjectLayer inLayer)
        : DefaultBroadPhaseLayerFilter(inObjectVsBroadPhaseLayerFilter, inLayer) {
    }

    bool ShouldCollide(BroadPhaseLayer inLayer) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer);
    }
};

class PySpecifiedBroadPhaseLayerFilter : public SpecifiedBroadPhaseLayerFilter {
  public:
    NB_TRAMPOLINE(SpecifiedBroadPhaseLayerFilter, 1);
    PySpecifiedBroadPhaseLayerFilter(BroadPhaseLayer inLayer) : SpecifiedBroadPhaseLayerFilter(inLayer) {
    }
    bool ShouldCollide(BroadPhaseLayer inLayer) const override {
        NB_OVERRIDE_NAME(
            "should_collide",
            ShouldCollide,
            inLayer);
    }
};

void BindBroadPhaseLayer(nb::module_ &m) {
    // Bind BroadPhaseLayer
    nb::class_<BroadPhaseLayer>(m, "BroadPhaseLayer",
        "An object layer can be mapped to a broadphase layer. Objects with the same broadphase layer will\n"
        "end up in the same sub structure (usually a tree) of the broadphase.\n"
        "When there are many layers, this reduces the total amount of sub structures the broad phase needs to manage.\n"
        "Usually you want objects that don't collide with each other\n"
        "in different broad phase layers, but there could be exceptions if objects layers only contain a minor amount of objects\n"
        "so it is not beneficial to give each layer its own sub structure in the broadphase.\n"
        "Note: This class requires explicit casting from and to Type to avoid confusion with ObjectLayer")
        .def(nb::init<>(), "Default constructor.")
        .def(nb::init<BroadPhaseLayer::Type>(), "value"_a, "Constructor from uint8 value.")
        .def("get_value", &BroadPhaseLayer::GetValue)

        .def(nb::self == nb::self, "Equality comparison.")
        .def(nb::self != nb::self, "Inequality comparison.")
        .def(nb::self < nb::self, "Less than comparison.")
        .def("__int__", &BroadPhaseLayer::GetValue)

        .def("__repr__", [](const BroadPhaseLayer &l) {
            return "BroadPhaseLayer(" + std::to_string(l.GetValue()) + ")";
        })
        .def("__hash__", [](const BroadPhaseLayer &l) {
            return std::hash<BroadPhaseLayer::Type>()(l.GetValue());
        });

    m.attr("BROAD_PHASE_LAYER_INVALID") = cBroadPhaseLayerInvalid;

    nb::class_<BroadPhaseLayerInterface, PyBroadPhaseLayerInterface>(m, "BroadPhaseLayerInterface",
        "Interface for mapping object layers to broad phase layers.")
        .def(nb::init<>())
        .def("get_num_broad_phase_layers", &BroadPhaseLayerInterface::GetNumBroadPhaseLayers, "Return the number of broadphase layers there are")
        .def("get_broad_phase_layer", &BroadPhaseLayerInterface::GetBroadPhaseLayer, "in_layer"_a,
            "Convert an object layer to the corresponding broadphase layer")
        .def("get_broad_phase_layer_name", &BroadPhaseLayerInterface::GetBroadPhaseLayerName, "in_layer"_a,
            "Get the user readable name of a broadphase layer (debugging purposes)");

    nb::class_<ObjectVsBroadPhaseLayerFilter, PyObjectVsBroadPhaseLayerFilter>(m, "ObjectVsBroadPhaseLayerFilter",
        "Filter class to test if an object can collide with a broadphase layer.")
        .def(nb::init<>())
        .def("should_collide", &ObjectVsBroadPhaseLayerFilter::ShouldCollide,
            "in_layer1"_a, "in_layer2"_a, "Returns true if an object layer should collide with a broadphase layer");

    nb::class_<BroadPhaseLayerFilter, PyBroadPhaseLayerFilter>(m, "BroadPhaseLayerFilter", "Filter class for broadphase layers in collision queries.")
        .def(nb::init<>())
        .def("should_collide", &BroadPhaseLayerFilter::ShouldCollide, "in_layer"_a,
            "Function to filter out broadphase layers when doing collision query test (return true to allow testing against objects with this layer)");

    nb::class_<DefaultBroadPhaseLayerFilter, PyDefaultBroadPhaseLayerFilter>(m, "DefaultBroadPhaseLayerFilter", "Default filter using ObjectVsBroadPhaseLayerFilter and ObjectLayer.")
        .def(nb::init<const ObjectVsBroadPhaseLayerFilter &, ObjectLayer>(), "object_vs_broad_phase_layer_filter"_a, "layer"_a)
        .def("should_collide", &DefaultBroadPhaseLayerFilter::ShouldCollide, "layer"_a);

    nb::class_<SpecifiedBroadPhaseLayerFilter, PySpecifiedBroadPhaseLayerFilter>(m, "SpecifiedBroadPhaseLayerFilter", "Allows objects from a specific broad phase layer only.")
        .def(nb::init<BroadPhaseLayer>(), "layer"_a)
        .def("should_collide", &SpecifiedBroadPhaseLayerFilter::ShouldCollide, "layer"_a);
}