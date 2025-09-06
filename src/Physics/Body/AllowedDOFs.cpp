#include "Common.h"
#include <Jolt/Physics/Body/AllowedDOFs.h>

void BindAllowedDOFs(nb::module_ &m) {
    nb::enum_<EAllowedDOFs>(m, "EAllowedDOFs", nb::is_arithmetic(),
        nb::is_arithmetic(),
        "Enum used in BodyCreationSettings and MotionProperties to indicate which degrees of freedom a body has")
        .value("NONE", EAllowedDOFs::None, "No degrees of freedom are allowed. Note that this is not valid and will crash. Use a static body instead.")
        .value("ALL", EAllowedDOFs::All, "All degrees of freedom are allowed")
        .value("TRANSLATION_X", EAllowedDOFs::TranslationX, "Body can move in world space X axis")
        .value("TRANSLATION_Y", EAllowedDOFs::TranslationY, "Body can move in world space Y axis")
        .value("TRANSLATION_Z", EAllowedDOFs::TranslationZ, "Body can move in world space Z axis")
        .value("ROTATION_X", EAllowedDOFs::RotationX, "Body can rotate around world space X axis")
        .value("ROTATION_Y", EAllowedDOFs::RotationY, "Body can rotate around world space Y axis")
        .value("ROTATION_Z", EAllowedDOFs::RotationZ, "Body can rotate around world space Z axis")
        .value("PLANE2D", EAllowedDOFs::Plane2D, "Body can only move in X and Y axis and rotate around Z axis");
}
