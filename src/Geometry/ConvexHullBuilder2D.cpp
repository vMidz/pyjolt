#include "Common.h"
#include <Jolt/Geometry/ConvexHullBuilder2D.h>

void BindConvexHullBuilder2D(nb::module_ &m) {
    nb::class_<ConvexHullBuilder2D, NonCopyable> convexHullBuilder2DCls(m, "ConvexHullBuilder2D",
        "A convex hull builder that tries to create 2D hulls as accurately as possible. Used for offline processing.");
    convexHullBuilder2DCls
        .def(nb::init<const ConvexHullBuilder2D::Positions &>(), "positions"_a, nb::keep_alive<1, 2>(),
            "Constructor.\n"
            "Args:\n"
            "    positions (Positions): Positions used to make the hull. Uses X and Y component of Vec3 only!.")

        .def("initialize", [](ConvexHullBuilder2D &self, int inIdx1, int inIdx2, int inIdx3, int inMaxVertices, float inTolerance) {
            ConvexHullBuilder2D::Edges outEdges;
            ConvexHullBuilder2D::EResult result = self.Initialize(inIdx1, inIdx2, inIdx3, inMaxVertices, inTolerance, outEdges);
            return nb::make_tuple(result, outEdges);
        },  "idx1"_a, "idx2"_a, "idx3"_a, "max_vertices"_a, "tolerance"_a,
            "Takes all positions as provided by the constructor and use them to build a hull\n"
            "Any points that are closer to the hull than inTolerance will be discarded.\n"
            "Args:\n"
            "    idx1 (int): , inIdx2 , inIdx3 The indices to use as initial hull (in any order).\n"
            "    max_vertices (int): Max vertices to allow in the hull. Specify INT_MAX if there is no limit.\n"
            "    tolerance (float): Max distance that a point is allowed to be outside of the hull.\n"
            "    edges (Edges): On success this will contain the list of indices that form the hull (counter clockwise).\n"
            "Returns:\n"
            "    EResult: Status code that reports if the hull was created or not.\n"
            "Output: (result_code, list_of_hull_vertex_indices)");

    nb::enum_<ConvexHullBuilder2D::EResult>(convexHullBuilder2DCls, "ConvexHullBuilder2DResult", "A convex hull builder that tries to create 2D hulls as accurately as possible. Used for offline processing.")
        .value("SUCCESS", ConvexHullBuilder2D::EResult::Success, "Hull building finished successfully")
        .value("MAX_VERTICES_REACHED", ConvexHullBuilder2D::EResult::MaxVerticesReached, "Hull building finished successfully, but the desired accuracy was not reached because the max vertices limit was reached");
}