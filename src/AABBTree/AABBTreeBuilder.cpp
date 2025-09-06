#include "Common.h"
#include <Jolt/AABBTree/AABBTreeBuilder.h>

void BindAABBTreeBuilder(nb::module_ &m) {
    nb::class_<AABBTreeBuilderStats> aabbTreeBuilderStatsCls(m, "AABBTreeBuilderStats");
    aabbTreeBuilderStatsCls
        .def(nb::init<>())
        .def_rw("splitter_stats", &AABBTreeBuilderStats::mSplitterStats, "stats"_a, "Stats returned by the triangle splitter algorithm")
        .def_rw("sah_cost", &AABBTreeBuilderStats::mSAHCost, "Surface Area Heuristic cost of this tree")
        .def_rw("min_depth", &AABBTreeBuilderStats::mMinDepth, "Minimal depth of tree (number of nodes)")
        .def_rw("max_depth", &AABBTreeBuilderStats::mMaxDepth, "Maximum depth of tree (number of nodes)")
        .def_rw("nodecount", &AABBTreeBuilderStats::mNodeCount, "Number of nodes in the tree")
        .def_rw("leafnode_count", &AABBTreeBuilderStats::mLeafNodeCount, "Number of leaf nodes (that contain triangles)")
        .def_rw("max_triangles_per_leaf", &AABBTreeBuilderStats::mMaxTrianglesPerLeaf, "Configured max triangles per leaf")
        .def_rw("treemin_triangles_per_leaf", &AABBTreeBuilderStats::mTreeMinTrianglesPerLeaf, "Minimal amount of triangles in a leaf")
        .def_rw("treemax_triangles_per_leaf", &AABBTreeBuilderStats::mTreeMaxTrianglesPerLeaf, "Maximal amount of triangles in a leaf")
        .def_rw("treeavg_triangles_per_leaf", &AABBTreeBuilderStats::mTreeAvgTrianglesPerLeaf, "Average amount of triangles in leaf nodes");

    nb::class_<AABBTreeBuilder> aabbTreeBuilderCls(m, "AABBTreeBuilder", "Helper class to build an AABB tree");
    aabbTreeBuilderCls
        .def(nb::init<TriangleSplitter &, uint>(), "in_splitter"_a, "max_triangles_per_leaf"_a = 16, "Constructor")
        .def("build", [](AABBTreeBuilder &self) {
            AABBTreeBuilderStats outStats;
            auto r = self.Build(outStats);
            return nb::make_tuple(r, outStats);
        }, "Recursively build tree, returns the root node of the tree\n"
           "Output: [root*, TreeBuilderStats]");

    nb::class_<AABBTreeBuilder::Node, NonCopyable>(aabbTreeBuilderCls, "Node",
        "A node in the tree, contains the AABox for the tree and any child nodes or triangles")
        .def(nb::init<>())
        .def("get_triangle_count", &AABBTreeBuilder::Node::GetTriangleCount, "Get number of triangles in this node")
        .def("gas_children", &AABBTreeBuilder::Node::HasChildren, "Check if this node has any children")
        .def("get_min_depth", &AABBTreeBuilder::Node::GetMinDepth, "Min depth of tree")
        .def("get_max_depth", &AABBTreeBuilder::Node::GetMaxDepth, "Max depth of tree")
        .def("get_node_count", &AABBTreeBuilder::Node::GetNodeCount, "Number of nodes in tree")
        .def("get_leaf_node_count", &AABBTreeBuilder::Node::GetLeafNodeCount, "Number of leaf nodes in tree")
        .def("get_triangle_count_in_tree", &AABBTreeBuilder::Node::GetTriangleCountInTree, "Get triangle count in tree")
        .def("get_triangle_count_per_node", [](AABBTreeBuilder::Node &self) {
            float average;
            uint min, max;
            self.GetTriangleCountPerNode(average, min, max);
            return nb::make_tuple(average, min, max);
        }, "Calculate min and max triangles per node\n"
           "Output: [average, min, max]")
        .def("calculate_sah_cost", &AABBTreeBuilder::Node::CalculateSAHCost,
            "cost_traversal"_a, "cost_leaf"_a, "Calculate the total cost of the tree using the surface area heuristic")
        .def("get_n_children", [](AABBTreeBuilder::Node &self, uint n) {
            Array<const AABBTreeBuilder::Node *> outChildren; // TODO:
            self.GetNChildren(n, outChildren);
            return outChildren;
        }, "n"_a, "Recursively get children (breadth first) to get in total N children (or less if there are no more)")

        .def_rw("bounds", &AABBTreeBuilder::Node::mBounds, "bounding_box"_a, "Bounding box")
        .def_rw("triangles", &AABBTreeBuilder::Node::mTriangles, "triangles"_a ,"Triangles (if no child nodes)")

        .def("get_child", [](AABBTreeBuilder::Node &self, int index) {  // TODO:
            if (index < 0 || index >= 2)
                throw nb::index_error("Index out of range");
            return self.mChild[index];
        }, "index"_a, "Child nodes (if no triangles)")
        .def("set_child", [](AABBTreeBuilder::Node &self, int index, AABBTreeBuilder::Node *value) {
            if (index < 0 || index >= 2)
                throw nb::index_error("Index out of range");            // TODO:
            self.mChild[index] = value;
        }, "index"_a, "value"_a, "Child nodes (if no triangles)");
}