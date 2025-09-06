#include "Common.h"
#include <Jolt/Geometry/Indexify.h>

void BindIndexify(nb::module_ &m) {
    m.def("indexify", [](const TriangleList &inTriangles, float inVertexWeldDistance = 1.0e-4f) {
        VertexList outVertices;
        IndexedTriangleList outTriangles;

        Indexify(inTriangles, outVertices, outTriangles, inVertexWeldDistance);
        return nb::make_tuple(outVertices, outTriangles);
    }, "triangles"_a, "vertex_weld_distance"_a = 1.0e-4f,
       "Take a list of triangles and get the unique set of vertices and use them to create indexed triangles.\n"
       "Vertices that are less than inVertexWeldDistance apart will be combined to a single vertex.\n"
       "Output: [outVertices, outTriangles]");

    m.def("indexify", [](const VertexList &inVertices, const IndexedTriangleList &inTriangles) {
        TriangleList outTriangles;
        Deindexify(inVertices, inTriangles, outTriangles);
        return outTriangles;
    }, "vertices"_a, "triangles"_a, "Take a list of indexed triangles and unpack them");
}