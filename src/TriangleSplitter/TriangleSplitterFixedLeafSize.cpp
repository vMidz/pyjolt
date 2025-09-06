#include "Common.h"
#include <Jolt/TriangleSplitter/TriangleSplitter.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/TriangleSplitter/TriangleSplitterFixedLeafSize.h>

void BindTriangleSplitterFixedLeafSize(nb::module_ &m) {
    nb::class_<TriangleSplitterFixedLeafSize, TriangleSplitter>(m,"TriangleSplitterFixedLeafSize",
        "Same as TriangleSplitterBinning, but ensuring that leaves have a fixed amount of triangles\n"
        "The resulting tree should be suitable for processing on GPU where we want all threads to process an equal amount of triangles")

        .def(nb::init<const VertexList &, const IndexedTriangleList &, uint, uint, uint, uint>(),
             "vertices"_a, "triangles"_a, "leaf_size"_a, "min_num_bins"_a = 8, "max_num_bins"_a = 128, "num_triangles_per_bin"_a = 6,
             "Constructor")

        .def("get_stats", [](TriangleSplitterFixedLeafSize &self) {
            TriangleSplitterFixedLeafSize::Stats out;
            self.GetStats(out);
            return out;
        })

        .def("split", [](TriangleSplitterFixedLeafSize &self, const TriangleSplitterFixedLeafSize::Range &inTriangles) {
            TriangleSplitterFixedLeafSize::Range outLeft;
            TriangleSplitterFixedLeafSize::Range outRight;
            self.Split(inTriangles, outLeft, outRight);
        }, "triangles"_a);
}