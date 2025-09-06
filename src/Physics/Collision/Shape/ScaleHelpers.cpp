#include "Common.h"
#include <Jolt/Physics/Collision/Shape/ScaleHelpers.h>

void BindScaleHelpers(nb::module_ &m) {
    using namespace ScaleHelpers;

    m.attr("MIN_SCALE") = cMinScale;
    m.attr("SCALE_TOLERANCE_SQ") = cScaleToleranceSq;

    m.def("is_not_scaled", &IsNotScaled, "scale"_a,
            "Test if a scale is identity");
    m.def("is_uniform_scale", &IsUniformScale, "scale"_a,
            "Test if a scale is uniform");
    m.def("is_uniform_scale_xz", &IsUniformScaleXZ, "scale"_a,
            "Test if a scale is uniform in XZ");
    m.def("scale_convex_radius", &ScaleConvexRadius, "convex_radius"_a, "scale"_a,
            "Scale the convex radius of an object");
    m.def("is_inside_out", &IsInsideOut, "scale"_a,
            "Test if a scale flips an object inside out (which requires flipping all normals and polygon windings)");
    m.def("is_zero_scale", &IsZeroScale, "scale"_a,
            "Test if any of the components of the scale have a value below cMinScale");
    m.def("make_non_zero_scale", &MakeNonZeroScale, "scale"_a,
            "Ensure that the scale for each component is at least cMinScale");
    m.def("make_uniform_scale", &MakeUniformScale, "scale"_a,
            "Get the average scale if inScale, used to make the scale uniform when a shape doesn't support non-uniform scale");
    m.def("make_uniform_scale_xz", &MakeUniformScaleXZ, "scale"_a,
            "Average the scale in XZ, used to make the scale uniform when a shape doesn't support non-uniform scale in the XZ plane");
    m.def("can_scale_be_rotated", &CanScaleBeRotated, "rotation"_a, "scale"_a,
        "Checks in scale can be rotated to child shape.\n"
        "Args:\n"
        "    rotation (Quat): Rotation of child shape.\n"
        "    scale (Vec3): Scale in local space of parent shape.\n"
        "Returns:\n"
        "    bool: True if the scale is valid (no shearing introduced).");

    m.def("rotate_scale", &RotateScale, "rotation"_a, "scale"_a,
        "Adjust scale for rotated child shape.\n"
        "Args:\n"
        "    rotation (Quat): Rotation of child shape.\n"
        "    scale (Vec3): Scale in local space of parent shape.\n"
        "Returns:\n"
        "    Vec3: Rotated scale.");
}
