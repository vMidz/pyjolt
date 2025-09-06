#include "Common.h"
#include <Jolt/Physics/Collision/CollectFacesMode.h>

void BindCollectFacesMode(nb::module_ &m) {
    nb::enum_<ECollectFacesMode>(m, "ECollectFacesMode",
        "Whether or not to collect faces, used by CastShape and CollideShape")
        .value("COLLECT_FACES", ECollectFacesMode::CollectFaces,
            "mShape1/2Face is desired")
        .value("NO_FACES", ECollectFacesMode::NoFaces,
            "mShape1/2Face is not desired");
}