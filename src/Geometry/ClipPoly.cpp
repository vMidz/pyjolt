#include "Common.h"
#include <Jolt/Geometry/ClipPoly.h>

void BindClipPoly(nb::module_ &m) {
    m.def("clip_poly_vs_plane", [](const Array<Vec3> &inPolygonToClip, Vec3Arg inPlaneOrigin, Vec3Arg inPlaneNormal) {
        Array<Vec3> outClippedPolygon;
        ClipPolyVsPlane(inPolygonToClip, inPlaneOrigin, inPlaneNormal, outClippedPolygon);
        return outClippedPolygon;
    }, "polygon_to_clip"_a, "plane_origin"_a, "plane_normal"_a,
       "Clip inPolygonToClip against the positive halfspace of plane defined by inPlaneOrigin and inPlaneNormal.\n"
       "inPlaneNormal does not need to be normalized.");

    m.def("clip_poly_vs_poly", [](const Array<Vec3> &inPolygonToClip, const Array<Vec3> &inClippingPolygon, Vec3Arg inClippingPolygonNormal) {
        Array<Vec3> outClippedPolygon;
        ClipPolyVsPoly(inPolygonToClip, inClippingPolygon, inClippingPolygonNormal, outClippedPolygon);
    }, "polygon_to_clip"_a, "clipping_polygon"_a, "clipping_polygon_normal"_a,
        "Clip polygon versus polygon.\n"
        "Both polygons are assumed to be in counter clockwise order.\n"
        "Args:\n"
        "    polygon_to_clip (Array::Vec3): is the polygon that is clipped.\n"
        "    clipping_polygon (Array::Vec3): is the polygon which inClippedPolygon is clipped against.\n"
        "    clipping_polygon_normal (Vec3): is used to create planes of all edges in inClippingPolygon against which inPolygonToClip is clipped, inClippingPolygonNormal does not need to be normalized.\n"
        "    clipped_polygon (Array::Vec3): will contain clipped polygon when function returns.");

    m.def("clip_poly_vs_edge", [](const Array<Vec3> &inPolygonToClip, Vec3Arg inEdgeVertex1, Vec3Arg inEdgeVertex2, Vec3Arg inClippingEdgeNormal) {
        Array<Vec3> outClippedPolygon;
        ClipPolyVsEdge(inPolygonToClip, inEdgeVertex1, inEdgeVertex2, inClippingEdgeNormal, outClippedPolygon);
        return outClippedPolygon;
    }, "polygon_to_clip"_a, "edge_vertex1"_a, "edge_vertex2"_a, "clipping_edge_normal"_a,
       "Clip inPolygonToClip against an edge, the edge is projected on inPolygonToClip using inClippingEdgeNormal.\n"
       "The positive half space (the side on the edge in the direction of inClippingEdgeNormal) is cut away.");

    m.def("clip_poly_vs_aa_box", [](const Array<Vec3> &inPolygonToClip, const AABox &inAABox) {
        Array<Vec3> outClippedPolygon;
        ClipPolyVsAABox(inPolygonToClip, inAABox, outClippedPolygon);
        return outClippedPolygon;
    }, "polygon_to_clip"_a, "aabbox"_a,
       "Clip polygon vs axis aligned box, inPolygonToClip is assume to be in counter clockwise order.\n"
       "Output will be stored in outClippedPolygon. Everything inside inAABox will be kept.");
}