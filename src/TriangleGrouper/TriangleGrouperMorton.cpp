#include "Common.h"
#include <Jolt/TriangleGrouper/TriangleGrouperMorton.h>

void BindTriangleGrouperMorton(nb::module_ &m) {
    nb::class_<TriangleGrouperMorton, TriangleGrouper>(m, "TriangleGrouperMorton",
        "A class that groups triangles in batches of N according to morton code of centroid.\n"
        "Time complexity: O(N log(N))")

        .def("group", [](TriangleGrouperMorton &self, const VertexList &inVertices, const IndexedTriangleList &inTriangles, int inGroupSize) {
            Array<uint> outGroupedTriangleIndices;
            self.Group(inVertices, inTriangles, inGroupSize, outGroupedTriangleIndices);
            return outGroupedTriangleIndices;
        }, "vertices"_a, "triangles"_a, "group_size"_a);
}