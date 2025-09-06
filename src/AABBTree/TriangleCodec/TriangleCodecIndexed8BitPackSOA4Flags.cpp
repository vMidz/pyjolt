#include "Common.h"
#include <nanobind/ndarray.h>
#include <Jolt/AABBTree/AABBTreeToBuffer.h>
#include <Jolt/AABBTree/TriangleCodec/TriangleCodecIndexed8BitPackSOA4Flags.h>

void BindTriangleCodecIndexed8BitPackSOA4Flags(nb::module_ &m) {
    using TriangleBlockFlag = nb::ndarray<nb::numpy, uint8, nb::shape<3, 4>>;
    using TriangleBlockIndices = nb::ndarray<nb::numpy, uint8, nb::shape<4>>;

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags> triangleCodecIndexed8BitPackSOA4FlagsCls(m,
    "TriangleCodecIndexed8BitPackSOA4Flags",
    "Store vertices in 64 bits and indices in 8 bits + 8 bit of flags per triangle like this:\n"
    "TriangleBlockHeader,\n"
    "TriangleBlock (4 triangles and their flags in 16 bytes),\n"
    "TriangleBlock...\n"
    "[Optional] UserData (4 bytes per triangle)\n"
    "Vertices are stored:\n"
    "VertexData (1 vertex in 64 bits),\n"
    "VertexData...\n"
    "They're compressed relative to the bounding box as provided by the node codec.");

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::TriangleHeader>(triangleCodecIndexed8BitPackSOA4FlagsCls, "TriangleHeader")
        .def(nb::init<>())
        .def_rw("offset", &TriangleCodecIndexed8BitPackSOA4Flags::TriangleHeader::mOffset, "Offset of all vertices")
        .def_rw("scale", &TriangleCodecIndexed8BitPackSOA4Flags::TriangleHeader::mScale,
            "Scale of all vertices, vertex_position = mOffset + mScale * compressed_vertex_position");

    triangleCodecIndexed8BitPackSOA4FlagsCls
        .def(nb::init<>())
        .def_ro_static("TRIANGLE_HEADER_SIZE", &TriangleCodecIndexed8BitPackSOA4Flags::TriangleHeaderSize,
            "Size of the header (an empty struct is always > 0 bytes so this needs a separate variable)")
        .def_ro_static("CHANGES_OFFSET_ON_PACK", &TriangleCodecIndexed8BitPackSOA4Flags::ChangesOffsetOnPack,
            "If this codec could return a different offset than the current buffer size when calling Pack()");

    nb::enum_<TriangleCodecIndexed8BitPackSOA4Flags::EComponentData>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "EComponentData", "Amount of bits per component")
        .value("COMPONENT_BITS", TriangleCodecIndexed8BitPackSOA4Flags::EComponentData::COMPONENT_BITS)
        .value("COMPONENT_MASK", TriangleCodecIndexed8BitPackSOA4Flags::EComponentData::COMPONENT_MASK);

    nb::enum_<TriangleCodecIndexed8BitPackSOA4Flags::EVertexXY>(triangleCodecIndexed8BitPackSOA4FlagsCls, "EVertexXY", "Packed X and Y coordinate")
        .value("COMPONENT_X", TriangleCodecIndexed8BitPackSOA4Flags::EVertexXY::COMPONENT_X)
        .value("COMPONENT_Y1", TriangleCodecIndexed8BitPackSOA4Flags::EVertexXY::COMPONENT_Y1)
        .value("COMPONENT_Y1_BITS", TriangleCodecIndexed8BitPackSOA4Flags::EVertexXY::COMPONENT_Y1_BITS);

    nb::enum_<TriangleCodecIndexed8BitPackSOA4Flags::EVertexZY>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "EVertexZY", "Packed Z and Y coordinate")
        .value("COMPONENT_Z", TriangleCodecIndexed8BitPackSOA4Flags::EVertexZY::COMPONENT_Z)
        .value("COMPONENT_Y2", TriangleCodecIndexed8BitPackSOA4Flags::EVertexZY::COMPONENT_Y2)
        .value("COMPONENT_Y2_BITS", TriangleCodecIndexed8BitPackSOA4Flags::EVertexZY::COMPONENT_Y2_BITS);

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::VertexData>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "VertexData", "A single packed vertex")
        .def(nb::init<>())
        .def_rw("vertex_xy", &TriangleCodecIndexed8BitPackSOA4Flags::VertexData::mVertexXY)
        .def_rw("vertex_zy", &TriangleCodecIndexed8BitPackSOA4Flags::VertexData::mVertexZY);

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlock>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "TriangleBlock", "A block of 4 triangles")
        .def(nb::init<>())
        .def_prop_rw("indices", [](TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlock &self) {
            return TriangleBlockIndices(self.mIndices, {3, 4}, nb::find(&self));
        }, [](TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlock &self, TriangleBlockIndices &other) {
                memcpy(&self.mIndices, other.data(), sizeof(self.mIndices));
            }, "8 bit indices to triangle vertices for 4 triangles in the form mIndices[vertex][triangle] where vertex in [0, 2] and triangle in [0, 3]")
        .def_prop_rw("flags", [](TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlock &self) {
            return TriangleBlockFlag(self.mFlags, {4}, nb::find(&self));
        }, [](TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlock &self, TriangleBlockFlag &other) {
                memcpy(&self.mFlags, other.data(), sizeof(self.mFlags));
            }, "Triangle flags (could contain material and active edges)");

    nb::enum_<TriangleCodecIndexed8BitPackSOA4Flags::ETriangleBlockHeaderFlags>(triangleCodecIndexed8BitPackSOA4FlagsCls, "ETriangleBlockHeaderFlags")
        .value("OFFSET_TO_VERTICES_BITS", TriangleCodecIndexed8BitPackSOA4Flags::ETriangleBlockHeaderFlags::OFFSET_TO_VERTICES_BITS,
            "Offset from current block to start of vertices in bytes")
        .value("OFFSET_TO_VERTICES_MASK", TriangleCodecIndexed8BitPackSOA4Flags::ETriangleBlockHeaderFlags::OFFSET_TO_VERTICES_MASK)
        .value("OFFSET_TO_USERDATA_BITS", TriangleCodecIndexed8BitPackSOA4Flags::ETriangleBlockHeaderFlags::OFFSET_TO_USERDATA_BITS,
            "When user data is stored, this is the number of blocks to skip to get to the user data (0 = no user data)")
        .value("OFFSET_TO_USERDATA_MASK", TriangleCodecIndexed8BitPackSOA4Flags::ETriangleBlockHeaderFlags::OFFSET_TO_USERDATA_MASK);

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlockHeader>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "TriangleBlockHeader", "A header for triangle blocks")
        .def(nb::init<>())
        .def("get_vertex_data", [](const TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlockHeader &self) {
            return self.GetVertexData();
        })
        .def("get_triangle_block", [](const TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlockHeader &self) {
            return self.GetTriangleBlock();
        })
        .def("get_user_data", [](const TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlockHeader &self) {
            const uint32_t *data = self.GetUserData();
            return nb::capsule(data, "uint32_t*"); // TODO:
        })
        .def_rw("flags", &TriangleCodecIndexed8BitPackSOA4Flags::TriangleBlockHeader::mFlags);

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::ValidationContext>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "ValidationContext", "This class is used to validate that the triangle data will not be degenerate after compression")
        .def(nb::init<const IndexedTriangleList &, const VertexList &>(), "triangle"_a, "vertices"_a)
        .def("is_degenerate", &TriangleCodecIndexed8BitPackSOA4Flags::ValidationContext::IsDegenerate, "triangle"_a,
            "Test if a triangle will be degenerate after quantization");

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::EncodingContext>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "EncodingContext", "Class to encode and compress triangle data into a byte buffer")
        .def(nb::init<const VertexList &>(), "vertices"_a)
        .def("get_pessimistic_memory_estimate",
             &TriangleCodecIndexed8BitPackSOA4Flags::EncodingContext::GetPessimisticMemoryEstimate,
             "triangle_count"_a,
             "store_user_data"_a,
             "Get an upper bound on the amount of bytes needed to store inTriangleCount triangles")
        .def("pack", [](TriangleCodecIndexed8BitPackSOA4Flags::EncodingContext &self,
            const IndexedTriangleList &triangles, bool inStoreUserData, ByteBuffer &ioBuffer) {
            const char *error;
            auto result = self.Pack(triangles, inStoreUserData, ioBuffer, error);
            return nb::make_tuple(result, error);
        }, "triangles"_a,
           "store_user_data"_a,
           "buffer"_a,
           "Pack the triangles in inContainer to ioBuffer. This stores the mMaterialIndex of a triangle in the 8 bit flags.\n"
           "Returns uint(-1) on error.\n       Output: [result, error]")

        .def("finalize", &TriangleCodecIndexed8BitPackSOA4Flags::EncodingContext::Finalize,
            "vertices"_a, "header"_a, "buffer"_a, "After all triangles have been packed, this finalizes the header and triangle buffer");

    nb::class_<TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext>(triangleCodecIndexed8BitPackSOA4FlagsCls,
        "DecodingContext", "This class is used to decode and decompress triangle data packed by the EncodingContext")
        .def("unpack", [](TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext &self, const void *inTriangleStart, uint32 inNumTriangles) {
            Vec3 *outTriangles;
            self.Unpack(inTriangleStart, inNumTriangles, outTriangles);
            return outTriangles;
        }, nb::rv_policy::reference_internal,
           "triagnle_start"_a,
           "num_triagnles"_a,
           "Unpacks triangles in the format t1v1,t1v2,t1v3, t2v1,t2v2,t2v3, ...")
        .def(nb::init<const TriangleCodecIndexed8BitPackSOA4Flags::TriangleHeader *>(), "header"_a)
        .def("unpack", [](TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext &self, const void *inTriangleStart, uint32 inNumTriangles) {
            Vec3 *outTriangles;
            uint8 *outTriangleFlags;
            self.Unpack(inTriangleStart, inNumTriangles, outTriangles, outTriangleFlags);
            nb::make_tuple(outTriangles, outTriangleFlags);
        }, "triangle_start"_a,
           "num_triangle"_a,
           "Unpacks triangles and flags, convenience function\n    Output: [outTriangles, outTriangleFlags]")

        .def("test_ray", [](TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext &self, Vec3Arg inRayOrigin,
            Vec3Arg inRayDirection, const void *inTriangleStart, uint32 inNumTriangles, float inClosest) {

            uint32 outClosestTriangleIndex;
            self.TestRay(inRayOrigin, inRayDirection, inTriangleStart, inNumTriangles, inClosest, outClosestTriangleIndex);
            return outClosestTriangleIndex;
        }, "ray_origin"_a,
           "ray_direction"_a,
           "triangle_start"_a,
           "num_triangle"_a,
           "closest"_a,
           "Tests a ray against the packed triangles\n  Output: [outClosestTriangleIndex]")

        .def("get_triangle", [](TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext &self,
            const void *inTriangleStart, uint32 inTriangleIdx) {

            Vec3 outV1, outV2, outV3;
            self.GetTriangle(inTriangleStart, inTriangleIdx, outV1, outV2, outV3);
            return nb::make_tuple(outV1, outV2, outV3);
        }, "triangle_start"_a,
           "triangle_index"_a,
           "Decode a single triangle\n  Output: [outV1, outV2, outV3]")

        .def("get_user_data", &TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext::GetUserData, "Get user data for a triangle")
        .def_static("get_flags", nb::overload_cast<const void *, uint32, uint8 *>(&TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext::sGetFlags),
            "triangle_start"_a, "num_triangle"_a, "triangle_flag"_a)
        .def_static("get_flags", nb::overload_cast<const void *, int>(&TriangleCodecIndexed8BitPackSOA4Flags::DecodingContext::sGetFlags),
            "triangle_start"_a, "triangle_index"_a);
}