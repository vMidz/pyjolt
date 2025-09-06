#include "Common.h"
#include <Jolt/Renderer/DebugRendererSimple.h>

#include <nanobind/trampoline.h>

class PyDebugRendererSimple : public DebugRenderer {
  public:
    NB_TRAMPOLINE(DebugRenderer, 3);

    ~PyDebugRendererSimple() override {
    }

    void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override {
        NB_OVERRIDE_NAME(
            "draw_line",
            DrawLine,
            inFrom,
            inTo,
            inColor);
    }

    void DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off) override {
        NB_OVERRIDE_NAME(
            "draw_triangle",
            DrawTriangle,
            inV1,
            inV2,
            inV3,
            inColor,
            inCastShadow);
    }

    void DrawText3D(RVec3Arg inPosition, const string_view &inString, ColorArg inColor = Color::sWhite, float inHeight = 0.5f) override {
        const char *c_str = &inString[0];
        NB_OVERRIDE_NAME(
            "draw_text3d",
            DrawText3D,
            inPosition,
            c_str,
            inColor,
            inHeight);
    }
};

void BindDebugRendererSimple(nb::module_ &m) {
    nb::class_<DebugRendererSimple, DebugRenderer> debugRendererSimpleCls(m, "DebugRendererSimple",
        "Inherit from this class to simplify implementing a debug renderer, start with this implementation:\n"
        "    class MyDebugRenderer : public JPH::DebugRendererSimple\n"
        "    {\n"
        "    public:\n"
        "        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override\n"
        "        {\n"
        "            // Implement\n"
        "        }\n"
        "        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override\n"
        "        {\n"
        "            // Implement\n"
        "        }\n"
        "        virtual void DrawText3D(JPH::RVec3Arg inPosition, const string_view &inString, JPH::ColorArg inColor, float inHeight) override\n"
        "        {\n"
        "            // Implement\n"
        "        }\n"
        "    };\n"
        "Note that this class is meant to be a quick start for implementing a debug renderer, it is not the most efficient way to implement a debug renderer.");
    debugRendererSimpleCls
        // .def(nb::init<>(),    // Abstract
            // "Constructor")
        .def("set_camera_pos", &DebugRendererSimple::SetCameraPos, "camera_pos"_a,
            "Should be called every frame by the application to provide the camera position.\n"
            "This is used to determine the correct LOD for rendering.")
        .def("draw_triangle", &DebugRendererSimple::DrawTriangle, "v1"_a, "v2"_a, "v3"_a, "color"_a, "cast_shadow"_a,
            "Fallback implementation that uses DrawLine to draw a triangle (override this if you have a version that renders solid triangles)");
}