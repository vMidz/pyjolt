#include "Common.h"
#include <Jolt/Physics/Collision/InternalEdgeRemovingCollector.h>
#include <Jolt/Physics/Body/Body.h>

void BindInternalEdgeRemovingCollector(nb::module_ &m) {
    nb::class_<InternalEdgeRemovingCollector, CollideShapeCollector> internalEdgeRemovingCollectorCls(m, "InternalEdgeRemovingCollector",
        "Removes internal edges from collision results. Can be used to filter out 'ghost collisions'.\n"
        "Based on: Contact generation for meshes - Pierre Terdiman (https://www.codercorner.com/MeshContacts.pdf)");
    internalEdgeRemovingCollectorCls
        .def(nb::init<CollideShapeCollector &>(), "chained_collector"_a,
            "Constructor, configures a collector to be called with all the results that do not hit internal edges")
        .def("reset", &InternalEdgeRemovingCollector::Reset)
        .def("on_body", &InternalEdgeRemovingCollector::OnBody, "body"_a)
        .def("add_hit", &InternalEdgeRemovingCollector::AddHit, "result"_a)
        .def("flush", &InternalEdgeRemovingCollector::Flush,
            "After all hits have been added, call this function to process the delayed results")
        .def_static("collide_shape_vs_shape", &InternalEdgeRemovingCollector::sCollideShapeVsShape,
            "shape1"_a, "shape2"_a, "scale1"_a, "scale2"_a, "center_of_mass_transform1"_a,
            "center_of_mass_transform2"_a, "sub_shape_id_creator1"_a, "sub_shape_id_creator2"_a,
            "collide_shape_settings"_a, "collector"_a, "shape_filter"_a,
            "Version of CollisionDispatch::sCollideShapeVsShape that removes internal edges");
}