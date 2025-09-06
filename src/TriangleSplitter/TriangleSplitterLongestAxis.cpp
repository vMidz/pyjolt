#include "Common.h"
#include <Jolt/TriangleSplitter/TriangleSplitterLongestAxis.h>

void BindTriangleSplitterLongestAxis(nb::module_ &m) {
    nb::class_<TriangleSplitterLongestAxis, TriangleSplitter>(m, "TriangleSplitterLongestAxis",
        "Splitter using center of bounding box with longest axis")

        .def(nb::init<const VertexList &, const IndexedTriangleList &>(),
            "vertices"_a, "triangles"_a,
            "Constructor")

        .def("get_stats", [](TriangleSplitterLongestAxis &self) {
            TriangleSplitterLongestAxis::Stats out;
            self.GetStats(out);
            return out;
        })

        .def("split", [](TriangleSplitterLongestAxis &self, const TriangleSplitterLongestAxis::Range &inTriangles) {
            TriangleSplitterLongestAxis::Range outLeft;
            TriangleSplitterLongestAxis::Range outRight;
            self.Split(inTriangles, outLeft, outRight);
        }, "triangles"_a);
}