#include "Common.h"
#include <Jolt/AABBTree/NodeCodec/NodeCodecQuadTreeHalfFloat.h>
#include <nanobind/ndarray.h>

// TODO:
struct TriangleContext{
};

// TODO:
struct Vistor{
    int VisitNodes(Vec4, Vec4, Vec4, Vec4, Vec4, Vec4, UVec4, int){
        return 0;
    }

    void VisitTriangles(const TriangleContext, const void *, uint32, uint32){

    }

    bool ShouldAbort(){
        return true;
    }

    bool ShouldVisitNode(int){
        return true;
    }
};

void BindNodeCodecQuadTreeHalfFloat(nb::module_ &m) {
    using NumpyHalfFloatArray4 = nb::ndarray<nb::numpy, uint16_t, nb::shape<4>>;
    using NumpyUint32Array4 = nb::ndarray<nb::numpy, uint32_t, nb::shape<4>>;

    using NodeCodecInst = NodeCodecQuadTreeHalfFloat<1>;

    nb::class_<NodeCodecInst> nodeCodecCls(m, "NodeCodecQuadTreeHalfFloatAlignment1");
    nodeCodecCls
        .def(nb::init<>())
        .def_ro_static("NUM_CHILDREN_PER_NODE", &NodeCodecInst::NumChildrenPerNode, "Number of child nodes of this node")
        .def_ro_static("HEADER_SIZE", &NodeCodecInst::HeaderSize, "Size of the header (an empty struct is always > 0 bytes so this needs a separate variable)")
        .def_ro_static("STACK_SIZE", &NodeCodecInst::StackSize, "Stack size to use during DecodingContext::sWalkTree")

        .def_prop_ro("TRIANGLE_COUNT_BITS", []() {return (uint32)NodeCodecInst::TRIANGLE_COUNT_BITS;})  // The cast is required for the .pyi stubs
        .def_prop_ro("TRIANGLE_COUNT_SHIFT", []() {return (uint32)NodeCodecInst::TRIANGLE_COUNT_SHIFT;})
        .def_prop_ro("TRIANGLE_COUNT_MASK", []() {return (uint32)NodeCodecInst::TRIANGLE_COUNT_MASK;})
        .def_prop_ro("OFFSET_BITS", []() {return (uint32)NodeCodecInst::OFFSET_BITS;})
        .def_prop_ro("OFFSET_MASK", []() {return (uint32)NodeCodecInst::OFFSET_MASK;})
        .def_prop_ro("OFFSET_NON_SIGNIFICANT_BITS", []() {return (uint32)NodeCodecInst::OFFSET_NON_SIGNIFICANT_BITS;})
        .def_prop_ro("OFFSET_NON_SIGNIFICANT_MASK", []() {return (uint32)NodeCodecInst::OFFSET_NON_SIGNIFICANT_MASK;});

    nb::class_<NodeCodecInst::Header> headerCls(nodeCodecCls, "Header", "Header for the tree");
    headerCls
        .def(nb::init<>())
        .def_rw("root_bounds_min", &NodeCodecInst::Header::mRootBoundsMin)
        .def_rw("root_bounds_max", &NodeCodecInst::Header::mRootBoundsMax)
        .def_rw("root_properties", &NodeCodecInst::Header::mRootProperties);

    nb::class_<NodeCodecInst::Node> NodeCls(nodeCodecCls, "Node", "Node structure");
    NodeCls
        .def(nb::init<>())
        .def_prop_rw("bounds_min_x", [](NodeCodecInst::Node &self) {
            return NumpyHalfFloatArray4(self.mBoundsMinX, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyHalfFloatArray4 &other) {
            memcpy(&self.mBoundsMinX, other.data(), sizeof(self.mBoundsMinX));
        }, "Min X bounds for 4 children (HalfFloat ndarray view)")

        .def_prop_rw("bounds_min_y", [](NodeCodecInst::Node &self) {
            return NumpyHalfFloatArray4(self.mBoundsMinY, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyHalfFloatArray4 &other) {
            memcpy(&self.mBoundsMinY, other.data(), sizeof(self.mBoundsMinY ));
        }, "Min Y bounds for 4 children (HalfFloat ndarray view)")

        .def_prop_rw("bounds_min_z", [](NodeCodecInst::Node &self) {
            return NumpyHalfFloatArray4(self.mBoundsMinZ, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyHalfFloatArray4 &other) {
            memcpy(&self.mBoundsMinZ, other.data(), sizeof(self.mBoundsMinZ));
        }, "Min Z bounds for 4 children (HalfFloat ndarray view)")

        .def_prop_rw("bounds_max_x", [](NodeCodecInst::Node &self) {
            return NumpyHalfFloatArray4(self.mBoundsMaxX, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyHalfFloatArray4 &other) {
            memcpy(&self.mBoundsMaxX, other.data(), sizeof(self.mBoundsMaxX));
        }, "Max X bounds for 4 children (HalfFloat ndarray view)")

        .def_prop_rw("bounds_max_y", [](NodeCodecInst::Node &self) {
            return NumpyHalfFloatArray4(self.mBoundsMaxY, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyHalfFloatArray4 &other) {
            memcpy(&self.mBoundsMaxY, other.data(), sizeof(self.mBoundsMaxY));
        }, "Max Y bounds for 4 children (HalfFloat ndarray view)")

        .def_prop_rw("bounds_max_z", [](NodeCodecInst::Node &self) {
            return NumpyHalfFloatArray4(self.mBoundsMaxZ, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyHalfFloatArray4 &other) {
            memcpy(&self.mBoundsMaxZ, other.data(), sizeof(self.mBoundsMaxZ));
        }, "Max Z bounds for 4 children (HalfFloat ndarray view)")

        .def_prop_rw("node_properties", [](NodeCodecInst::Node &self) {
            return NumpyUint32Array4(self.mNodeProperties, {4}, nb::find(&self));
        }, [](NodeCodecInst::Node &self, NumpyUint32Array4 &other) {
            memcpy(&self.mNodeProperties, other.data(), sizeof(self.mNodeProperties));
        }, "Max Z bounds for 4 children (HalfFloat ndarray view)");

    nb::class_<NodeCodecInst::EncodingContext> encodingContextCls(nodeCodecCls, "EncodingContext", "Encodes and compresses quad tree nodes");
    encodingContextCls
        .def(nb::init<>())
        .def("get_pessimistic_memory_estimate", &NodeCodecInst::EncodingContext::GetPessimisticMemoryEstimate,
            "in_node_count"_a, "Get an upper bound on the amount of bytes needed for a node tree with inNodeCount nodes")
        .def("node_allocate", [](NodeCodecInst::EncodingContext &self, const AABBTreeBuilder::Node *inNode, Vec3Arg inNodeBoundsMin,
            Vec3Arg inNodeBoundsMax, Array<const AABBTreeBuilder::Node *> &ioChildren,
            Vec3 outChildBoundsMin[NodeCodecQuadTreeHalfFloat<1>::NumChildrenPerNode],
            Vec3 outChildBoundsMax[NodeCodecQuadTreeHalfFloat<1>::NumChildrenPerNode],
            ByteBuffer &ioBuffer) {

            const char *outError;
            auto status = self.NodeAllocate(inNode, inNodeBoundsMin, inNodeBoundsMax,
                ioChildren, outChildBoundsMin, outChildBoundsMax, ioBuffer, outError);
            return nb::make_tuple(status, outError);
        }, "Allocate a new node for inNode.\n"
           "Algorithm can modify the order of ioChildren to indicate in which order children should be compressed\n"
           "Algorithm can enlarge the bounding boxes of the children during compression and returns these in outChildBoundsMin, outChildBoundsMax\n"
           "inNodeBoundsMin, inNodeBoundsMax is the bounding box if inNode possibly widened by compressing the parent node\n"
           "Returns uint(-1) on error and reports the error in outError\n"
           "Output: [status, error_msg]")

        .def("node_finalize", [](NodeCodecInst::EncodingContext &self, const AABBTreeBuilder::Node *inNode,
                uint inNodeStart, uint inNumChildren, const uint *inChildrenNodeStart,
                const uint *inChildrenTrianglesStart, ByteBuffer &ioBuffer){

            const char *outError;
            auto status = self.NodeFinalize(inNode, inNodeStart, inNumChildren, inChildrenNodeStart, inChildrenTrianglesStart, ioBuffer, outError);
            return nb::make_tuple(status, outError);

        }, "Once all nodes have been added, this call finalizes all nodes by patching in the offsets of the child nodes (that were added after the node itself was added)\n"
           "Output: [status, out_error]")
        .def("finalize", [](NodeCodecInst::EncodingContext &self, const AABBTreeBuilder::Node *inRoot,
            uint inRootTrianglesStart, uint inRootNodeStart){

            NodeCodecInst::Header *outHeader;
            const char *outError;
            auto status = self.Finalize(outHeader, inRoot, inRootNodeStart, inRootTrianglesStart, outError);
            return nb::make_tuple(status, outHeader, outError);
        }, "Once all nodes have been finalized, this will finalize the header of the nodes\n"
           "Output:[status, out_header, out_error]");

    nb::class_<NodeCodecInst::DecodingContext> decodingContextCls(nodeCodecCls, "DecodingContext", "Decodes and decompresses quad tree nodes");
    decodingContextCls
        .def_static("triangle_block_id_bits", &NodeCodecInst::DecodingContext::sTriangleBlockIDBits,
            "in_tree"_a, "Get the amount of bits needed to store an ID to a triangle block")

        .def_static("get_triangle_block_start", [](const uint8 *inBufferStart, uint inTriangleBlockID){
            NodeCodecInst::DecodingContext::sGetTriangleBlockStart(inBufferStart, inTriangleBlockID);
        }, "Convert a triangle block ID to the start of the triangle buffer")

        .def(nb::init<const typename NodeCodecInst::Header *>(), "in_header"_a, "Initializes the context with the tree header")

        .def("walk_tree", &NodeCodecInst::DecodingContext::WalkTree<TriangleContext, Vistor>,
            "Walk the node tree calling the Visitor::VisitNodes for each node encountered and Visitor::VisitTriangles for each triangle encountered")
        .def("is_done_walking", &NodeCodecInst::DecodingContext::IsDoneWalking,
            "This can be used to have the visitor early out (ioVisitor.ShouldAbort() returns true) and later continue again (call WalkTree() again)");
}
