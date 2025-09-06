#include "Common.h"
#include <Jolt/TriangleGrouper/TriangleGrouperClosestCentroid.h>

void BindTriangleGrouperClosestCentroid(nb::module_ &m) {
    nb::class_<TriangleGrouperClosestCentroid, TriangleGrouper>(m, "TriangleGrouperMorton",
        "A class that groups triangles in batches of N."
        "Starts with centroid with lowest X coordinate and finds N closest centroids, this repeats until all groups have been found.\n"
        "Time complexity: O(N^2)")

        .def("group", [](TriangleGrouperClosestCentroid &self, const VertexList &inVertices, const IndexedTriangleList &inTriangles, int inGroupSize) {
            Array<uint> outGroupedTriangleIndices;
            self.Group(inVertices, inTriangles, inGroupSize, outGroupedTriangleIndices);
            return outGroupedTriangleIndices;
        }, "vertices"_a, "triangles"_a, "group_size"_a);
}