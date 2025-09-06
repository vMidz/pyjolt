#include "Common.h"
#include <Jolt/AABBTree/AABBTreeToBuffer.h>
#include <Jolt/AABBTree/AABBTreeBuilder.h>
#include <Jolt/AABBTree/TriangleCodec/TriangleCodecIndexed8BitPackSOA4Flags.h>
#include <Jolt/AABBTree/NodeCodec/NodeCodecQuadTreeHalfFloat.h>

void BindAABBTreeToBuffer(nb::module_ &m) {
    using TriangleCodec = TriangleCodecIndexed8BitPackSOA4Flags;
    using NodeCodec = NodeCodecQuadTreeHalfFloat<1>;
    using AABBTreeToBufferInst = AABBTreeToBuffer<TriangleCodec, NodeCodec>;

    nb::class_<AABBTreeToBufferInst>(m, "AABBTreeToBuffer",
        "Conversion algorithm that converts an AABB tree to an optimized binary buffer")
        .def(nb::init<>())
        .def_ro_static("HEADER_SIZE", &AABBTreeToBufferInst::HeaderSize, "Size in bytes of the header of the tree.")
        .def_ro_static("NUM_CHILDREN_PER_NODE", &AABBTreeToBufferInst::NumChildrenPerNode, "Maximum number of children per node in the tree.")
        .def_ro_static("TRIANGLE_HEADER_SIZE", &AABBTreeToBufferInst::TriangleHeaderSize, "Size in bytes of the header for the triangles.")

        .def("convert", [](AABBTreeToBufferInst &self, const VertexList &inVertices, const AABBTreeBuilder::Node *inRoot, bool inStoreUserData) {
            const char *error_msg;
            bool status = self.Convert(inVertices, inRoot, inStoreUserData, error_msg);
            return nb::make_tuple(status, error_msg);
        },  "in_vertices"_a,
            "in_root"_a,
            "in_store_user_data"_a,
            "Convert AABB tree. Returns false if failed.\n"
            "Output: [result, error_msg]")

        .def("get_buffer", nb::overload_cast<>(&AABBTreeToBufferInst::GetBuffer),
            "Get resulting data")

        .def("get_node_header", &AABBTreeToBufferInst::GetNodeHeader, "Get header for tree")
        .def("get_triangle_header", &AABBTreeToBufferInst::GetTriangleHeader, "Get header for triangles")
        .def("get_root", [](AABBTreeToBufferInst &self) {
            auto ptr = self.GetRoot();                 // TODO:
            return nb::capsule(ptr, "void_ptr");
        }, "Get root of resulting tree");
}