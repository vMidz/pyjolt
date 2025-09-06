#include "Common.h"
#include <Jolt/TriangleSplitter/TriangleSplitterMorton.h>

void BindTriangleSplitterMorton(nb::module_ &m) {
    nb::class_<TriangleSplitterMorton, TriangleSplitter>(m, "TriangleSplitterMorton",
        "Splitter using Morton codes, see: http://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/")

        .def(nb::init<const VertexList &, const IndexedTriangleList &>(),
            "vertices"_a, "triangles"_a,
            "Constructor")

        .def("get_stats", [](TriangleSplitterMorton &self) {
            TriangleSplitterMorton::Stats out;
            self.GetStats(out);
            return out;
        })

        .def("split", [](TriangleSplitterMorton &self, const TriangleSplitterMorton::Range &inTriangles) {
            TriangleSplitterMorton::Range outLeft;
            TriangleSplitterMorton::Range outRight;
            self.Split(inTriangles, outLeft, outRight);
        }, "triangles"_a);
}