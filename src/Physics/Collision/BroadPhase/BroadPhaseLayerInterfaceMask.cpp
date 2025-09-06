#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h>

void BindBroadPhaseLayerInterfaceMask(nb::module_ &m) {
    nb::class_<BroadPhaseLayerInterfaceMask, BroadPhaseLayerInterface> broadPhaseLayerInterfaceMaskCls(m, "BroadPhaseLayerInterfaceMask",
        "BroadPhaseLayerInterface implementation.\n"
        "This defines a mapping between object and broadphase layers.\n"
        "This implementation works together with ObjectLayerPairFilterMask and ObjectVsBroadPhaseLayerFilterMask.\n"
        "A broadphase layer is suitable for an object if its group & inGroupsToInclude is not zero and its group & inGroupsToExclude is zero.\n"
        "The broadphase layers are iterated from lowest to highest value and the first one that matches is taken. If none match then it takes the last layer.");
    broadPhaseLayerInterfaceMaskCls
        .def(nb::init<uint>(), "num_broad_phase_layers"_a)
        .def("configure_layer", &BroadPhaseLayerInterfaceMask::ConfigureLayer, "broad_phase_layer"_a, "groups_to_include"_a, "groups_to_exclude"_a)
        .def("get_num_broad_phase_layers", &BroadPhaseLayerInterfaceMask::GetNumBroadPhaseLayers)
        .def("get_broad_phase_layer", &BroadPhaseLayerInterfaceMask::GetBroadPhaseLayer, "layer"_a)
        .def("should_collide", &BroadPhaseLayerInterfaceMask::ShouldCollide, "layer1"_a, "layer2"_a,
            "Returns true if an object layer should collide with a broadphase layer, this function is being called from ObjectVsBroadPhaseLayerFilterMask")
        .def("set_broad_phase_layer_name", &BroadPhaseLayerInterfaceMask::SetBroadPhaseLayerName, "layer"_a, "name"_a)
        .def("get_broad_phase_layer_name", &BroadPhaseLayerInterfaceMask::GetBroadPhaseLayerName, "layer"_a, nb::rv_policy::reference);
}