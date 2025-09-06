#include "Common.h"
#include <Jolt/Geometry/ClosestPoint.h>

using namespace ClosestPoint;

void BindClosestPoint(nb::module_ &m) {
    m.def("get_bary_centric_coordinates", [](Vec3Arg inA, Vec3Arg inB) {
        float outU;
        float outV;
        bool r = GetBaryCentricCoordinates(inA, inB, outU, outV);
        return nb::make_tuple(r, outU, outV);
    }, "a"_a, "b"_a,
       "Compute barycentric coordinates of closest point to origin for infinite line defined by (inA, inB)\n"
       "Point can then be computed as inA * outU + inB * outV\n"
       "Returns false if the points inA, inB do not form a line (are at the same point)\n"
       "Output:[status, U, V]"
    );

    m.def("get_bary_centric_coordinates", [](Vec3Arg inA, Vec3Arg inB, Vec3Arg inC) {
        float outU, outV, outW;
        bool r = GetBaryCentricCoordinates(inA, inB, inC, outU, outV, outW);
        return nb::make_tuple(r, outU, outV, outW);
    }, "a"_a, "b"_a, "c"_a,
       "Compute barycentric coordinates of closest point to origin for plane defined by (inA, inB, inC)\n"
       "Point can then be computed as inA * outU + inB * outV + inC * outW\n"
       "Returns false if the points inA, inB, inC do not form a plane (are on the same line or at the same point)\n"
       "Output: [status, outU, outV, outW]"

    );

    m.def("get_closest_point_on_triangle", [](Vec3Arg inA, Vec3Arg inB, Vec3Arg inC, bool MustIncludeC) {
        uint32 outSet;
        Vec3 r;
        if (MustIncludeC)
            r = GetClosestPointOnTriangle<true>(inA, inB, inC, outSet);
        else
            r = GetClosestPointOnTriangle<false>(inA, inB, inC, outSet);

        return nb::make_tuple(r, outSet);
    }, "a"_a, "b"_a, "c"_a, "must_include_c"_a,
       "Get the closest point to the origin of triangle (inA, inB, inC)\n"
       "outSet describes which features are closest: 1 = a, 2 = b, 4 = c, 5 = line segment ac, 7 = triangle interior etc.\n"
       "If MustIncludeC is true, the function assumes that C is part of the closest feature (vertex, edge, face) and does less work, if the assumption is not true then a closest point to the other features is returned.\n"
       "Output:[Vec3, outSet]");

    m.def("get_closest_point_on_line", [](Vec3Arg inA, Vec3Arg inB) {
        uint32 outSet;
        auto r = GetClosestPointOnLine(inA, inB, outSet);
        return nb::make_tuple(r, outSet);
    }, "a"_a, "b"_a,
       "Get the closest point to the origin of line (inA, inB)\n"
        "outSet describes which features are closest: 1 = a, 2 = b, 3 = line segment ab\n     Output:[Vec3, set]");

    m.def("origin_outside_of_tetrahedron_planes", &OriginOutsideOfTetrahedronPlanes,
        "a"_a, "b"_a, "c"_a, "d"_a,
        "Returns for each of the planes of the tetrahedron if the origin is inside it\n"
        "Roughly equivalent to:\n"
        "[OriginOutsideOfPlane(inA, inB, inC, inD),\n"
        "OriginOutsideOfPlane(inA, inC, inD, inB),\n"
        "OriginOutsideOfPlane(inA, inD, inB, inC),\n"
        "OriginOutsideOfPlane(inB, inD, inC, inA)]");
}