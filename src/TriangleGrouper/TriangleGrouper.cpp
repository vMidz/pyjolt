#include "Common.h"
#include <Jolt/TriangleGrouper/TriangleGrouper.h>

void BindTriangleGrouper(nb::module_ &m) {
    nb::class_<TriangleGrouper, NonCopyable> triangleGrouperCls(m, "TriangleGrouper",
        "A class that groups triangles in batches of N (according to closeness)");
    triangleGrouperCls
        .def("group", [](TriangleGrouper &self, const VertexList &inVertices, const IndexedTriangleList &inTriangles, int inGroupSize){
            Array<uint> outGroupedTriangleIndices;
            self.Group(inVertices, inTriangles, inGroupSize, outGroupedTriangleIndices);
            return outGroupedTriangleIndices;
        }, "vertices"_a, "triangles"_a, "group_size"_a,
            "Group a batch of indexed triangles.\n"
            "Args:\n"
            "    vertices (VertexList): The list of vertices.\n"
            "    triangles (IndexedTriangleList): The list of indexed triangles (indexes into inVertices).\n"
            "    group_size (int): How big each group should be.\n"
            "    grouped_triangle_indices (Array<int>): An ordered list of indices (indexing into inTriangles), contains groups of inGroupSize large worth of indices to triangles that are grouped together. If the triangle count is not an exact multiple of inGroupSize the last batch will be smaller.");
}
