#include "Common.h"
#include <Jolt/Renderer/DebugRendererRecorder.h>

#include <nanobind/stl/string.h>

void BindDebugRendererRecorder(nb::module_ &m) {
    nb::class_<DebugRendererRecorder, DebugRenderer> debugRendererRecorderCls(m, "DebugRendererRecorder",
        "Implementation of DebugRenderer that records the API invocations to be played back later",
            nb::is_final());
    debugRendererRecorderCls
        .def(nb::init<StreamOut &>(), "stream"_a,
            "Constructor")
        .def("draw_line", &DebugRendererRecorder::DrawLine, "from_"_a, "to"_a, "color"_a,
            "Implementation of DebugRenderer interface")
        .def("draw_triangle", &DebugRendererRecorder::DrawTriangle, "v1"_a, "v2"_a, "v3"_a, "color"_a, "cast_shadow"_a)
        // .def("create_triangle_batch", nb::overload_cast<>(&DebugRendererRecorder::CreateTriangleBatch), "triangles"_a, "triangle_count"_a)
        // .def("create_triangle_batch", &DebugRendererRecorder::CreateTriangleBatch, "vertices"_a, "vertex_count"_a, "indices"_a, "index_count"_a)
        .def("draw_geometry", &DebugRendererRecorder::DrawGeometry, "model_matrix"_a, "world_space_bounds"_a, "lod_scale_sq"_a, "model_color"_a, "geometry"_a, "cull_mode"_a, "cast_shadow"_a, "draw_mode"_a)
        .def("draw_text3_d", [](DebugRendererRecorder &self, RVec3Arg inPosition, const char *inString, ColorArg inColor, float inHeight){
            self.DrawText3D(inPosition, inString, inColor, inHeight);
        }, "position"_a, "string"_a, "color"_a, "height"_a)
        .def("end_frame", &DebugRendererRecorder::EndFrame,
            "Mark the end of a frame");

    nb::enum_<DebugRendererRecorder::ECommand>(debugRendererRecorderCls, "ECommand",
            "Control commands written into the stream")
        .value("CREATE_BATCH", DebugRendererRecorder::ECommand::CreateBatch)
        .value("CREATE_BATCH_INDEXED", DebugRendererRecorder::ECommand::CreateBatchIndexed)
        .value("CREATE_GEOMETRY", DebugRendererRecorder::ECommand::CreateGeometry)
        .value("END_FRAME", DebugRendererRecorder::ECommand::EndFrame);

    nb::class_<DebugRendererRecorder::LineBlob> lineBlobCls(debugRendererRecorderCls, "LineBlob",
        "Holds a single line segment");
    lineBlobCls
        .def(nb::init<>())
        .def_rw("from_", &DebugRendererRecorder::LineBlob::mFrom)
        .def_rw("to", &DebugRendererRecorder::LineBlob::mTo)
        .def_rw("color", &DebugRendererRecorder::LineBlob::mColor);

    nb::class_<DebugRendererRecorder::TriangleBlob> triangleBlobCls(debugRendererRecorderCls, "TriangleBlob",
        "Holds a single triangle");
    triangleBlobCls
        .def(nb::init<>())
        .def_rw("v1", &DebugRendererRecorder::TriangleBlob::mV1)
        .def_rw("v2", &DebugRendererRecorder::TriangleBlob::mV2)
        .def_rw("v3", &DebugRendererRecorder::TriangleBlob::mV3)
        .def_rw("color", &DebugRendererRecorder::TriangleBlob::mColor)
        .def_rw("cast_shadow", &DebugRendererRecorder::TriangleBlob::mCastShadow);

    nb::class_<DebugRendererRecorder::TextBlob> textBlobCls(debugRendererRecorderCls, "TextBlob",
        "Holds a single text entry");
    textBlobCls
        .def(nb::init<>())
        .def("__init__", [](DebugRendererRecorder::TextBlob &self, RVec3Arg inPosition, const char *inString, ColorArg inColor, float inHeight){
            new (&self) DebugRendererRecorder::TextBlob(inPosition, inString, inColor, inHeight);
        }, "position"_a, "string"_a, "color"_a, "height"_a)
        .def_rw("position", &DebugRendererRecorder::TextBlob::mPosition)
        .def_rw("string", &DebugRendererRecorder::TextBlob::mString)
        .def_rw("color", &DebugRendererRecorder::TextBlob::mColor)
        .def_rw("height", &DebugRendererRecorder::TextBlob::mHeight);

    nb::class_<DebugRendererRecorder::GeometryBlob> geometryBlobCls(debugRendererRecorderCls, "GeometryBlob",
        "Holds a single geometry draw call");
    geometryBlobCls
        .def(nb::init<>())
        .def_rw("model_matrix", &DebugRendererRecorder::GeometryBlob::mModelMatrix)
        .def_rw("model_color", &DebugRendererRecorder::GeometryBlob::mModelColor)
        .def_rw("geometry_id", &DebugRendererRecorder::GeometryBlob::mGeometryID)
        .def_rw("cull_mode", &DebugRendererRecorder::GeometryBlob::mCullMode)
        .def_rw("cast_shadow", &DebugRendererRecorder::GeometryBlob::mCastShadow)
        .def_rw("draw_mode", &DebugRendererRecorder::GeometryBlob::mDrawMode);

    nb::class_<DebugRendererRecorder::Frame> frameCls(debugRendererRecorderCls, "Frame",
        "All information for a single frame");
    frameCls
        .def(nb::init<>())
        .def_rw("lines", &DebugRendererRecorder::Frame::mLines)
        .def_rw("triangles", &DebugRendererRecorder::Frame::mTriangles)
        .def_rw("texts", &DebugRendererRecorder::Frame::mTexts)
        .def_rw("geometries", &DebugRendererRecorder::Frame::mGeometries);
}
