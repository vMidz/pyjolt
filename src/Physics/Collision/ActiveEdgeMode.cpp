#include "Common.h"
#include <Jolt/Physics/Collision/ActiveEdgeMode.h>

void BindActiveEdgeMode(nb::module_ &m) {
    nb::enum_<EActiveEdgeMode>(m, "EActiveEdgeMode",
        "How to treat active/inactive edges.\n"
        "An active edge is an edge that either has no neighbouring edge or if the angle between the two connecting faces is too large, see: ActiveEdges")
        .value("COLLIDE_ONLY_WITH_ACTIVE", EActiveEdgeMode::CollideOnlyWithActive,
            "Do not collide with inactive edges. For physics simulation, this gives less ghost collisions.")
        .value("COLLIDE_WITH_ALL", EActiveEdgeMode::CollideWithAll,
            "Collide with all edges. Use this when you're interested in all collisions.");
}