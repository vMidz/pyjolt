#include "Common.h"
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>

void BindBroadPhaseLayerInterfaceTable(nb::module_ &m) {
    nb::class_<BroadPhaseLayerInterfaceTable, BroadPhaseLayerInterface> broadPhaseLayerInterfaceTableCls(m, "BroadPhaseLayerInterfaceTable",
        "BroadPhaseLayerInterface implementation.\n"
        "This defines a mapping between object and broadphase layers.\n"
        "This implementation uses a simple table");
    broadPhaseLayerInterfaceTableCls
        .def(nb::init<uint, uint>(), "num_object_layers"_a, "num_broad_phase_layers"_a)
        .def("map_object_to_broad_phase_layer", &BroadPhaseLayerInterfaceTable::MapObjectToBroadPhaseLayer, "object_layer"_a, "broad_phase_layer"_a)
        .def("get_num_broad_phase_layers", &BroadPhaseLayerInterfaceTable::GetNumBroadPhaseLayers)
        .def("get_broad_phase_layer", &BroadPhaseLayerInterfaceTable::GetBroadPhaseLayer, "layer"_a)
        .def("set_broad_phase_layer_name", &BroadPhaseLayerInterfaceTable::SetBroadPhaseLayerName, "layer"_a, "name"_a)
        .def("get_broad_phase_layer_name", &BroadPhaseLayerInterfaceTable::GetBroadPhaseLayerName, "layer"_a, nb::rv_policy::reference);
}