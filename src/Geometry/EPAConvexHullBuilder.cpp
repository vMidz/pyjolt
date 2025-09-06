#include "Common.h"
#include <Jolt/Geometry/EPAConvexHullBuilder.h>

void BindEPAConvexHullBuilder(nb::module_ &m) {
    nb::class_<EPAConvexHullBuilder> ch_builder(m, "EPAConvexHullBuilder", "A convex hull builder specifically made for the EPA penetration depth calculation. It trades accuracy for speed and will simply abort of the hull forms defects due to numerical precision problems.");
    ch_builder
        .def_ro_static("MAX_TRIANGLES", &EPAConvexHullBuilder::cMaxTriangles, "Max triangles in hull")
        .def_ro_static("MAX_POINTS", &EPAConvexHullBuilder::cMaxPoints, "Max number of points in hull")
        .def_ro_static("max_edge_length", &EPAConvexHullBuilder::cMaxEdgeLength, "Max number of edges in FindEdge")
        .def_ro_static("c_min_triangle_area", &EPAConvexHullBuilder::cMinTriangleArea, "Minimum area of a triangle before, if smaller than this it will not be added to the priority queue")
        .def_ro_static("c_barycentric_epsilon", &EPAConvexHullBuilder::cBarycentricEpsilon, "Epsilon value used to determine if a point is in the interior of a triangle")

        .def(nb::init<const EPAConvexHullBuilder::Points &>(), "Constructor")
        .def("initialize", &EPAConvexHullBuilder::Initialize, "idx1"_a, "idx2"_a, "idx3"_a, "Initialize the hull with 3 points")
        .def("has_next_triangle", &EPAConvexHullBuilder::HasNextTriangle, "Check if there's another triangle to process from the queue")
        .def("peek_closest_triangle_in_queue", &EPAConvexHullBuilder::PeekClosestTriangleInQueue, nb::rv_policy::reference_internal, "Access to the next closest triangle to the origin and remove it from the queue.")
        .def("pop_closest_triangle_from_queue", &EPAConvexHullBuilder::PopClosestTriangleFromQueue, nb::rv_policy::reference_internal, "Access to the next closest triangle to the origin and remove it from the queue.")
        .def("find_facing_triangle", [](EPAConvexHullBuilder &self, Vec3Arg inPosition) {
            float outBestDistSq = 0.0f;
            EPAConvexHullBuilder::Triangle *triangle = self.FindFacingTriangle(inPosition, outBestDistSq);
            return nb::make_tuple(*triangle, outBestDistSq);
        },  "position"_a,
            nb::rv_policy::reference_internal,
            "Find the triangle on which inPosition is the furthest to the front\n"
            "Note this function works as long as all points added have been added with AddPoint(..., FLT_MAX).\n"
            "Output: [triangle, bestDistSq]")
        .def("add_point", [](EPAConvexHullBuilder &self, EPAConvexHullBuilder::Triangle *inFacingTriangle, int inIdx, float inClosestDistSq) {
            using NewTrianglesType = StaticArray<EPAConvexHullBuilder::Triangle *, EPAConvexHullBuilder::cMaxEdgeLength>;
            NewTrianglesType outTriangles;
            bool success = self.AddPoint(inFacingTriangle, inIdx, inClosestDistSq, outTriangles);
            std::vector<EPAConvexHullBuilder::Triangle *> return_triangles;
            if (success) {
                return_triangles.assign(outTriangles.begin(), outTriangles.end());
            }
            return nb::make_tuple(success, return_triangles);
        },  "facing_triangle"_a,
            "index"_a,
            "closest_dist_sq"_a,
            nb::rv_policy::reference_internal,
            "Add a new point to the convex hull\n  Output: [status, triangles]")
        .def("free_triangle", &EPAConvexHullBuilder::FreeTriangle, "triangle"_a, "Free a triangle");

    nb::class_<EPAConvexHullBuilder::Edge>(ch_builder, "Edge", "Class that holds the information of an edge")
        .def_rw("neighbour_triangle", &EPAConvexHullBuilder::Edge::mNeighbourTriangle, "Triangle that neighbours this triangle")
        .def_rw("neighbour_edge", &EPAConvexHullBuilder::Edge::mNeighbourEdge, "Index in mEdge that specifies edge that this Edge is connected to")
        .def_rw("start_idx", &EPAConvexHullBuilder::Edge::mStartIdx, "Vertex index in mPositions that indicates the start vertex of this edge");

    nb::class_<EPAConvexHullBuilder::Triangle, NonCopyable>(ch_builder, "Triangle", "Class that holds the information of one triangle")
        .def(nb::init<int, int, int, const Vec3 *>(), "Constructor")
        .def("is_facing", &EPAConvexHullBuilder::Triangle::IsFacing, "position"_a, "Check if triangle is facing inPosition")
        .def("is_facing_origin", &EPAConvexHullBuilder::Triangle::IsFacingOrigin, "Check if triangle is facing the origin")
        .def("get_next_edge", &EPAConvexHullBuilder::Triangle::GetNextEdge, "Get the next edge of edge inIndex")
        // .def_rw("edge", &EPAConvexHullBuilder::Triangle::mEdge,"3 edges of this triangle") // TODO:
        .def_rw("normal", &EPAConvexHullBuilder::Triangle::mNormal, "Normal of this triangle, length is 2 times area of triangle")
        .def_rw("centroid", &EPAConvexHullBuilder::Triangle::mCentroid, "Center of the triangle")
        .def_rw("closest_len_sq", &EPAConvexHullBuilder::Triangle::mClosestLenSq, "Closest distance^2 from origin to triangle")
        // .def_rw("lambda", &EPAConvexHullBuilder::Triangle::mLambda,"Barycentric coordinates of closest point to origin on triangle") // TODO:
        .def_rw("lambda_relative_to0", &EPAConvexHullBuilder::Triangle::mLambdaRelativeTo0, "How to calculate the closest point, true: y0 + l0 * (y1 - y0) + l1 * (y2 - y0), false: y1 + l0 * (y0 - y1) + l1 * (y2 - y1)")
        .def_rw("closest_point_interior", &EPAConvexHullBuilder::Triangle::mClosestPointInterior, "Flag that indicates that the closest point from this triangle to the origin is an interior point")
        .def_rw("removed", &EPAConvexHullBuilder::Triangle::mRemoved, "Flag that indicates that triangle has been removed")
        .def_rw("in_queue", &EPAConvexHullBuilder::Triangle::mInQueue, "Flag that indicates that this triangle was placed in the sorted heap (stays true after it is popped because the triangle is freed by the main EPA algorithm loop)")
        // .def_rw("iteration", &EPAConvexHullBuilder::Triangle::mIteration)

        .def("__repr__", [](const EPAConvexHullBuilder::Triangle &self) {
            return nb::str("EPAConvexHullBuilderTriangle(v=({}, {}, {}), removed={})")
                .format(self.mEdge[0].mStartIdx, self.mEdge[1].mStartIdx, self.mEdge[2].mStartIdx, self.mRemoved);
        });

    nb::class_<EPAConvexHullBuilder::TriangleFactory>(ch_builder, "TriangleFactory", "Factory that creates triangles in a fixed size buffer")
        .def("claer", &EPAConvexHullBuilder::TriangleFactory::Clear, "Return all triangles to the free pool")
        .def("create_triangle", &EPAConvexHullBuilder::TriangleFactory::CreateTriangle, "Allocate a new triangle with 3 indexes")
        .def("free_triangle", &EPAConvexHullBuilder::TriangleFactory::FreeTriangle, "Free a triangle");

    using PointsBase = StaticArray<Vec3, EPAConvexHullBuilder::cMaxPoints>;

    nb::class_<EPAConvexHullBuilder::Points, PointsBase>(ch_builder, "Points", "Specialized points list that allows direct access to the size")
        .def("get_ref_size", &EPAConvexHullBuilder::Points::GetSizeRef);

    nb::class_<EPAConvexHullBuilder::TriangleQueue, EPAConvexHullBuilder::Triangles>(ch_builder, "TriangleQueue",
        "Specialized triangles list that keeps them sorted on closest distance to origin")
        .def_static("triangle_sorter", &EPAConvexHullBuilder::TriangleQueue::sTriangleSorter, "Function to sort triangles on closest distance to origin")
        .def("push_back", &EPAConvexHullBuilder::TriangleQueue::push_back, "Add triangle to the list")
        .def("peek_closest", &EPAConvexHullBuilder::TriangleQueue::PeekClosest, "Peek the next closest triangle without removing it")
        .def("pop_closest", &EPAConvexHullBuilder::TriangleQueue::PopClosest, "Get next closest triangle");
}