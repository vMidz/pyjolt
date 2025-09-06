#include "Common.h"
#include <Jolt/TriangleSplitter/TriangleSplitterMean.h>

void BindTriangleSplitterMean(nb::module_ &m) {
    nb::class_<TriangleSplitterMean, TriangleSplitter>(m, "TriangleSplitterMean",
        "Splitter using mean of axis with biggest centroid deviation")

        .def(nb::init<const VertexList, const IndexedTriangleList>(),
            "vertices"_a, "triangles"_a,
            "Constructor")

        .def("get_stats", [](TriangleSplitterMean &self) {
            TriangleSplitterMean::Stats out;
            self.GetStats(out);
            return out;
        })

        .def("split", [](TriangleSplitterMean &self, const TriangleSplitterMean::Range &inTriangles) {
            TriangleSplitterMean::Range outLeft;
            TriangleSplitterMean::Range outRight;
            self.Split(inTriangles, outLeft, outRight);
        }, "triangles"_a);
}