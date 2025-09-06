#include "Common.h"
#include <Jolt/TriangleSplitter/TriangleSplitterBinning.h>

void BindTriangleSplitterBinning(nb::module_ &m) {
    nb::class_<TriangleSplitterBinning, TriangleSplitter>(m, "TriangleSplitterBinning",
        "Binning splitter approach taken from: Realtime Ray Tracing on GPU with BVH-based Packet Traversal by Johannes Gunther et al.")
        .def(nb::init<const VertexList &, const IndexedTriangleList &, uint, uint, uint>(),
             "vertices"_a, "triangles"_a, "min_num_bins"_a = 8, "max_num_bins"_a = 128, "num_triangles_per_bin"_a = 6,
             "Constructor")
        .def("get_stats", [](TriangleSplitterBinning &self) {
            TriangleSplitterBinning::Stats out;
            self.GetStats(out);
            return out;
        })
        .def("split", [](TriangleSplitterBinning &self, const TriangleSplitterBinning::Range &inTriangles) {
            TriangleSplitterBinning::Range outLeft;
            TriangleSplitterBinning::Range outRight;
            self.Split(inTriangles, outLeft, outRight);
        }, "triangles"_a);
}