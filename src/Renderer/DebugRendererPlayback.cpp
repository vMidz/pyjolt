#include "Common.h"
#include <Jolt/Renderer/DebugRendererPlayback.h>

void BindDebugRendererPlayback(nb::module_ &m) {
    nb::class_<DebugRendererPlayback> debugRendererPlaybackCls(m, "DebugRendererPlayback",
            "Class that can read a recorded stream from DebugRendererRecorder and plays it back trough a DebugRenderer");
    debugRendererPlaybackCls
        .def(nb::init<DebugRenderer &>(), "renderer"_a,
            "Constructor")
        .def("parse", &DebugRendererPlayback::Parse, "stream"_a,
            "Parse a stream of frames")
        .def("get_num_frames", &DebugRendererPlayback::GetNumFrames,
            "Get the number of parsed frames")
        .def("draw_frame", &DebugRendererPlayback::DrawFrame, "frame_number"_a,
            "Draw a frame");
}