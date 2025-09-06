#include "Common.h"
#include <Jolt/Geometry/AABox4.h>

void BindAABox4(nb::module_ &m) {
    m.def("aa_box4_vs_box", nb::overload_cast<const AABox &, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg>(&AABox4VsBox),
          "box1", "box2_min_x", "box2_min_y", "box2_min_z", "box2_max_x", "box2_max_y", "box2_max_z",
          "Helper functions that process 4 axis aligned boxes at the same time using SIMD\n"
          "Test if 4 bounding boxes overlap with 1 bounding box, splat 1 box");

    m.def("aa_box4_vs_box", nb::overload_cast<Mat44Arg, Vec3Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, float>(&AABox4VsBox),
          "orientation"_a, "half_extents"_a, "box_min_x"_a, "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a, "inEpsilon"_a = 1.0e-6f,
          "Test if 4 bounding boxes overlap with an oriented box");

    m.def("aa_box4_vs_box", nb::overload_cast<const OrientedBox &, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, float>(&AABox4VsBox),
          "box"_a, "box_min_x"_a, "vec4_arg"_a "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a, "epsilon"_a = 1.0e-6f,
          "Convenience function that tests 4 AABoxes vs OrientedBox");

    m.def("aa_box4_scale", [](Vec3Arg inScale, Vec4Arg inBoxMinX, Vec4Arg inBoxMinY, Vec4Arg inBoxMinZ,
        Vec4Arg inBoxMaxX, Vec4Arg inBoxMaxY, Vec4Arg inBoxMaxZ) {

        Vec4 outBoundsMinX;
        Vec4 outBoundsMinY;
        Vec4 outBoundsMinZ;
        Vec4 outBoundsMaxX;
        Vec4 outBoundsMaxY;
        Vec4 outBoundsMaxZ;
        AABox4Scale(inScale, inBoxMinX, inBoxMinY, inBoxMinZ, inBoxMaxX, inBoxMaxY,
            inBoxMaxZ, outBoundsMinX, outBoundsMinY, outBoundsMinZ, outBoundsMaxX, outBoundsMaxY, outBoundsMaxZ);
        nb::make_tuple(outBoundsMinX, outBoundsMinY, outBoundsMinZ, outBoundsMaxX, outBoundsMaxY, outBoundsMaxZ);
    }, "Scale 4 axis aligned boxes\n"
       "Output[outBoundsMinX, outBoundsMinY, outBoundsMinZ, outBoundsMaxX, outBoundsMaxY, outBoundsMaxZ]");

    m.def("aa_box4_enlarge_with_extent", &AABox4EnlargeWithExtent,
          "extent"_a, "bounds_min_x"_a, "bounds_min_y"_a, "bounds_min_z"_a,
          "bounds_max_x"_a, "bounds_max_y"_a, "bounds_max_z"_a,
          "Enlarge 4 bounding boxes with extent (add to both sides)");

    m.def("aa_box4_vs_point", &AABox4VsPoint,
          "point"_a, "box_min_x"_a, "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a,
          "Test if 4 bounding boxes overlap with a point");

    m.def("aa_box4_distance_sq_to_point", nb::overload_cast<Vec4Arg, Vec4Arg, Vec4Arg,
        Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg>(&AABox4DistanceSqToPoint),
          "point_x"_a, "point_y"_a, "point_z"_a,
          "box_min_x"_a, "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a,
          "Get the squared distance between 4 AABoxes and a point");

    m.def("aa_box4_distance_sq_to_point", nb::overload_cast<Vec3, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg>(&AABox4DistanceSqToPoint),
          "point"_a, "box_min_x"_a, "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a,
          "Get the squared distance between 4 AABoxes and a point");

    m.def("aa_box4_vs_sphere", nb::overload_cast<Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg,
        Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg>(&AABox4VsSphere),
          "center_x"_a, "center_y"_a, "center_z"_a,
          "radius_sq"_a, "box_min_x"_a, "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a,
          "Test 4 AABoxes vs a sphere");

    m.def("aa_box4_vs_sphere", nb::overload_cast<Vec3Arg, float, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg, Vec4Arg>(&AABox4VsSphere),
          "center"_a, "radius_sq"_a,
          "box_min_x"_a, "box_min_y"_a, "box_min_z"_a,
          "box_max_x"_a, "box_max_y"_a, "box_max_z"_a,
          "Test 4 AABoxes vs a sphere");
}
