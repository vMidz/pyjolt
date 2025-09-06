#include "Common.h"
#include <Jolt/TriangleSplitter/TriangleSplitter.h>
#include <Jolt/Geometry/IndexedTriangle.h>
#include <Jolt/TriangleSplitter/TriangleSplitterBinning.h>

void BindTriangleSplitter(nb::module_ &m) {
    nb::class_<TriangleSplitter, NonCopyable> triangleSplitterCls(m, "TriangleSplitter",
        "A class that splits a triangle list into two parts for building a tree");
    triangleSplitterCls
        // .def(nb::init<const VertexList &, const IndexedTriangleList &>(), "vertices"_a, "triangles"_a, // Abstract class
        //     "Constructor")
        .def("get_stats", &TriangleSplitter::GetStats, "stats"_a,
            "Get stats of splitter")
        .def("get_initial_range", &TriangleSplitter::GetInitialRange,
            "Range of triangles to start with")
        .def("split", &TriangleSplitter::Split, "triangles"_a, "left"_a, "right"_a,
            "Split triangles into two groups left and right, returns false if no split could be made.\n"
            "Args:\n"
            "    triangles (Range): The range of triangles (in mSortedTriangleIdx) to process.\n"
            "    left (Range): On return this will contain the ranges for the left subpart. mSortedTriangleIdx may have been shuffled.\n"
            "    right (Range): On return this will contain the ranges for the right subpart. mSortedTriangleIdx may have been shuffled.\n"
            "Returns:\n"
            "    bool: Returns true when a split was found.")
        .def("get_vertices", &TriangleSplitter::GetVertices,
            "Get the list of vertices")
        .def("get_triangle", &TriangleSplitter::GetTriangle, "idx"_a,
            "Get triangle by index");

    nb::class_<TriangleSplitter::Stats> statsCls(triangleSplitterCls, "Stats");
    statsCls
        .def_rw("splitter_name", &TriangleSplitter::Stats::mSplitterName)
        .def_rw("leaf_size", &TriangleSplitter::Stats::mLeafSize);

    nb::class_<TriangleSplitter::Range> rangeCls(triangleSplitterCls, "Range",
        "Helper struct to indicate triangle range before and after the split");
    rangeCls
        .def(nb::init<>(),
            "Constructor")
        .def(nb::init<uint, uint>(), "begin"_a, "end"_a)
        .def("count", &TriangleSplitter::Range::Count,
            "Get number of triangles in range")
        .def_rw("begin", &TriangleSplitter::Range::mBegin,
            "Start and end index (end = 1 beyond end)")
        .def_rw("end", &TriangleSplitter::Range::mEnd);
}