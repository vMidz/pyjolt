#include "Common.h"
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Geometry/OrientedBox.h>
#include <Jolt/Core/Mutex.h>

#include <nanobind/trampoline.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/string.h>
#include <BindingUtility/Tuple.h>
#include <unordered_map>

#define NB_OVERRIDE_NAME_CUSTOM_ARGS(name, func, returnType, ...)          \
    using nb_ret_type = returnType;                                        \
    nanobind::detail::ticket nb_ticket(nb_trampoline, name, false);        \
    if (nb_ticket.key.is_valid()) {                                        \
        return nanobind::cast<nb_ret_type>(                                \
            nb_trampoline.base().attr(nb_ticket.key)(__VA_ARGS__));        \
    } else                                                                 \
        nb::raise_python_error();

#define NB_OVERRIDE_PURE_NAME_CUSTOM_ARGS(name, func, returnType, ...)         \
    using nb_ret_type = returnType;                                            \
    nanobind::detail::ticket nb_ticket(nb_trampoline, name, true);             \
    return nanobind::cast<nb_ret_type>(                                        \
        nb_trampoline.base().attr(nb_ticket.key)(__VA_ARGS__))

class BatchImpl : public RefTargetVirtual
{
public:
    virtual void AddRef() override { ++mRefCount; }
    virtual void Release() override
    {
        if (--mRefCount == 0)
            delete this;
    }

    int geometryIndex;
    BatchImpl() =default;

    BatchImpl(const BatchImpl& other) : geometryIndex(other.geometryIndex), mRefCount(0)
    {
    }

    // Copy assignment operator
    BatchImpl& operator=(const BatchImpl& other)
    {
        if (this != &other)
            geometryIndex = other.geometryIndex;
        return *this;
    }

    atomic<uint32> mRefCount = 0;
};

static Mutex linesLock;
static Mutex trianglesLock;
static Mutex textsLock;

class PyDebugRenderer : public DebugRenderer {
  public:
    NB_TRAMPOLINE(DebugRenderer, 6);

    PyDebugRenderer(){
        Initialize();
    }

    ~PyDebugRenderer() override {
    }

    void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override {
        lock_guard lock(linesLock);

        NB_OVERRIDE_PURE_NAME(
            "draw_line",
            DrawLine,
            inFrom,
            inTo,
            inColor);
    }

    void DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off) override {
        lock_guard lock(trianglesLock);

        NB_OVERRIDE_PURE_NAME(
            "draw_triangle",
            DrawTriangle,
            inV1,
            inV2,
            inV3,
            inColor,
            inCastShadow);
    }

    void DrawText3D(RVec3Arg inPosition, const string_view &inString, ColorArg inColor = Color::sWhite, float inHeight = 0.5f) override {
        lock_guard lock(textsLock);

        std::string str(inString);
        auto c_str = str.c_str();
        NB_OVERRIDE_PURE_NAME(
            "draw_text3d",
            DrawText3D,
            inPosition,
            c_str,
            inColor,
            inHeight);
    }

    virtual void DrawGeometry(RMat44Arg inModelMatrix, const AABox &inWorldSpaceBounds, float inLODScaleSq, ColorArg inModelColor,
                              const GeometryRef &inGeometry, ECullMode inCullMode = ECullMode::CullBackFace,
                              ECastShadow inCastShadow = ECastShadow::On, EDrawMode inDrawMode = EDrawMode::Solid) override {
        lock_guard lock(trianglesLock);

        NB_OVERRIDE_PURE_NAME(
            "draw_geometry",
            DrawGeometry,
            inModelMatrix,
            inWorldSpaceBounds,
            inLODScaleSq,
            inModelColor,
            inGeometry,
            inCullMode,
            inCastShadow,
            inDrawMode);
    }

    virtual Batch CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) override {
        Array<Triangle> array(inTriangles, inTriangles + inTriangleCount);

        nanobind::detail::ticket nb_ticket(nb_trampoline, "create_triangle_batch_triangles", true);
        int pyReturnValue = nanobind::cast<int>(nb_trampoline.base().attr(nb_ticket.key)(array));

        auto batch = new BatchImpl;
        batch->geometryIndex = pyReturnValue;
        return batch;
    }

    virtual Batch CreateTriangleBatch(const Vertex *inVertices, int inVertexCount, const uint32 *inIndices, int inIndexCount) override {
        using NumpyIndices= nb::ndarray<nb::numpy,const uint32_t, nb::shape<-1>, nb::device::cpu, nb::c_contig>;

        Array<Vertex> vertexArray(inVertices, inVertices + inVertexCount);
        auto numpyIndices= NumpyIndices(inIndices, {(uint32_t)inIndexCount});

        nanobind::detail::ticket nb_ticket(nb_trampoline, "create_triangle_batch", true);
        int pyReturnValue = nanobind::cast<int>(nb_trampoline.base().attr(nb_ticket.key)(vertexArray, numpyIndices));

        auto batch = new BatchImpl;
        batch->geometryIndex = pyReturnValue;
        return batch;
    }
};

// TODO: relocate
struct VertexHash
{
    std::size_t operator()(const DebugRenderer::Vertex& vertex) const
    {
        std::size_t h1 = std::hash<float>()(vertex.mPosition[0]);
        std::size_t h2 = std::hash<float>()(vertex.mPosition[1]);
        std::size_t h3 = std::hash<float>()(vertex.mPosition[2]);
        std::size_t h4 = std::hash<float>()(vertex.mNormal[0]);
        std::size_t h5 = std::hash<float>()(vertex.mNormal[1]);
        std::size_t h6 = std::hash<float>()(vertex.mNormal[2]);
        std::size_t h7 = std::hash<float>()(vertex.mUV.x);
        std::size_t h8 = std::hash<float>()(vertex.mUV.y);
        std::size_t h9 = std::hash<int>()(vertex.mColor.mU32);

        // Combine hashes
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^
               (h6 << 5) ^ (h7 << 6) ^ (h8 << 7) ^ (h9 << 8);
    }
};

// Equality comparison for vertices with tolerance
struct VertexEqual
{
    float tolerance;

    VertexEqual(float tol = 1e-6f) : tolerance(tol) {}

    bool operator()(const DebugRenderer::Vertex& a, const DebugRenderer::Vertex& b) const
    {
        // Compare positions
        for(int i = 0; i < 3; i++) {
            if(std::abs(a.mPosition[i] - b.mPosition[i]) > tolerance) return false;
        }

        // Compare normals
        for(int i = 0; i < 3; i++) {
            if(std::abs(a.mNormal[i] - b.mNormal[i]) > tolerance) return false;
        }

        // Compare UV coordinates
        if(std::abs(a.mUV.x - b.mUV.x) > tolerance) return false;
        if(std::abs(a.mUV.y - b.mUV.y) > tolerance) return false;

        // Compare color (exact match for integer)
        return a.mColor == b.mColor;
    }
};

nb::tuple CreateIndexBufferFromTriangleArray(Array<DebugRenderer::Triangle> &triangles, float tolerance = 1e-6f) {
    if (triangles.empty()) {
        return nb::make_tuple();
    }

    // Flatten array
    Array<DebugRenderer::Vertex> vertices;
    for(const auto &e : triangles)
    {
        vertices.push_back(e.mV[0]);
        vertices.push_back(e.mV[1]);
        vertices.push_back(e.mV[2]);
    }

    Array<uint32_t> indices;
    Array<DebugRenderer::Vertex> uniqueVertices;

    // Use unordered_map for fast lookups
    std::unordered_map<DebugRenderer::Vertex, unsigned int, VertexHash, VertexEqual> vertexMap(
        0, VertexHash(), VertexEqual(tolerance));

    indices.reserve(vertices.size());
    uniqueVertices.reserve(vertices.size()); // Reserve maximum possible size

    for (const auto &vertex : vertices) {
        auto it = vertexMap.find(vertex);

        if (it != vertexMap.end()) {
            // Vertex already exists, use existing index
            indices.push_back(it->second);
        } else {
            // New unique vertex
            unsigned int newIndex = static_cast<unsigned int>(uniqueVertices.size());
            uniqueVertices.push_back(vertex);
            vertexMap[vertex] = newIndex;
            indices.push_back(newIndex);
        }
    }

    return CreateTuple(uniqueVertices, indices);
}

void BindDebugRenderer(nb::module_ &m) {
    nb::class_<BatchImpl>(m, "BatchImpl")
        .def(nb::init<>())
        .def_rw("geometry_index", &BatchImpl::geometryIndex);

    nb::class_<DebugRenderer, PyDebugRenderer> debugRendererCls(m, "DebugRenderer",
        "Simple triangle renderer for debugging purposes.\n"
        "Inherit from this class to provide your own implementation.\n"
        "Implement the following virtual functions:\n"
        "- DrawLine\n"
        "- DrawTriangle\n"
        "- DrawText3D\n"
        "- CreateTriangleBatch\n"
        "- DrawGeometry\n"
        "Make sure you call Initialize() from the constructor of your implementation.\n"
        "The CreateTriangleBatch is used to prepare a batch of triangles to be drawn by a single DrawGeometry call,\n"
        "which means that Jolt can render a complex scene much more efficiently than when each triangle in that scene would have been drawn through DrawTriangle.\n"
        "Note that an implementation that implements CreateTriangleBatch and DrawGeometry is provided by DebugRendererSimple which can be used to start quickly.\n");

    debugRendererCls
        .def(nb::init<>(), "Constructor")
        .def("next_frame", &DebugRenderer::NextFrame,
            "Call once after frame is complete. Releases unused dynamically generated geometry assets.")
        .def("draw_line", &DebugRenderer::DrawLine, "from_"_a, "to"_a, "color"_a,
            "Draw line")
        .def("draw_wire_box", nb::overload_cast<const AABox &, ColorArg>(&DebugRenderer::DrawWireBox),
            "box"_a, "color"_a,
            "Draw wireframe box")
        .def("draw_wire_box", nb::overload_cast<const OrientedBox &, ColorArg>(&DebugRenderer::DrawWireBox), "box"_a, "color"_a)
        .def("draw_wire_box", nb::overload_cast<RMat44Arg, const AABox &, ColorArg>(&DebugRenderer::DrawWireBox), "matrix"_a, "box"_a, "color"_a)
        .def("draw_marker", &DebugRenderer::DrawMarker, "position"_a, "color"_a, "size"_a,
            "Draw a marker on a position")
        .def("draw_arrow", &DebugRenderer::DrawArrow, "from_"_a, "to"_a, "color"_a, "size"_a,
            "Draw an arrow")
        .def("draw_coordinate_system", &DebugRenderer::DrawCoordinateSystem,
            "transform"_a, "size"_a = 1.0f,
            "Draw coordinate system (3 arrows, x = red, y = green, z = blue)")

        .def("draw_plane", &DebugRenderer::DrawPlane,
            "point"_a, "normal"_a, "color"_a, "size"_a,
            "Draw a plane through inPoint with normal inNormal")

        .def("draw_wire_triangle", &DebugRenderer::DrawWireTriangle,
            "v1"_a, "v2"_a, "v3"_a, "color"_a, "Draw wireframe triangle")

        .def("draw_wire_polygon",&DebugRenderer::DrawWirePolygon<Array<Vec3>>, "transform"_a, "vertices"_a, "color"_a, "arrow_size"_a = 0.0f,
            "Draw a wireframe polygon")

        .def("draw_wire_sphere", nb::overload_cast<RVec3Arg, float, ColorArg, int>(&DebugRenderer::DrawWireSphere),
            "center"_a, "radius"_a, "color"_a, "level"_a = 3,
            "Draw wireframe sphere")

        .def("draw_wire_unit_sphere", &DebugRenderer::DrawWireUnitSphere, "matrix"_a, "color"_a, "level"_a = 3);

    nb::enum_<DebugRenderer::ECastShadow>(debugRendererCls, "ECastShadow", "Enum that determines if a shadow should be cast or not")
        .value("ON", DebugRenderer::ECastShadow::On, "This shape should cast a shadow")
        .value("OFF", DebugRenderer::ECastShadow::Off, "This shape should not cast a shadow");

    nb::enum_<DebugRenderer::EDrawMode>(debugRendererCls, "EDrawMode", "Determines how triangles are drawn")
        .value("SOLID", DebugRenderer::EDrawMode::Solid, "Draw as a solid shape")
        .value("WIREFRAME", DebugRenderer::EDrawMode::Wireframe, "Draw as wireframe");

    nb::enum_<DebugRenderer::ECullMode>(debugRendererCls, "ECullMode", "Determines which polygons are culled")
        .value("CULL_BACK_FACE", DebugRenderer::ECullMode::CullBackFace, "Don't draw backfacing polygons")
        .value("CULL_FRONT_FACE", DebugRenderer::ECullMode::CullFrontFace, "Don't draw front facing polygons")
        .value("OFF", DebugRenderer::ECullMode::Off, "Don't do culling and draw both sides");

    nb::class_<DebugRenderer::Vertex>(debugRendererCls, "Vertex", "Vertex format used by the triangle renderer")
        .def(nb::init<>())
        .def_rw("position", &DebugRenderer::Vertex::mPosition)
        .def_rw("normal", &DebugRenderer::Vertex::mNormal)
        .def_rw("uv", &DebugRenderer::Vertex::mUV)
        .def_rw("color", &DebugRenderer::Vertex::mColor);

    constexpr int VERTEX_COUNT = _countof(Triangle::mV);

    nb::class_<DebugRenderer::Triangle>(debugRendererCls, "Triangle", "A single triangle")
        .def(nb::init<>())
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg, ColorArg>(), "v1"_a, "v2"_a, "v3"_a, "color"_a)
        .def(nb::init<Vec3Arg, Vec3Arg, Vec3Arg, ColorArg, Vec3Arg, Vec3Arg>(), "v1"_a, "v2"_a, "v3"_a, "color"_a, "uv_origin"_a, "uv_direction"_a)

        .def("__getitem__", [VERTEX_COUNT](const DebugRenderer::Triangle &v, int i) -> const DebugRenderer::Vertex & {
            if (i < 0)
                i += VERTEX_COUNT; // negative indexing support

            if (i < 0 || i >= VERTEX_COUNT)
                throw nb::index_error("Index out of range");

            return v.mV[i];
        }, nb::is_operator(), nb::rv_policy::reference_internal)

        .def("__setitem__", [VERTEX_COUNT](DebugRenderer::Triangle &t, int i, const DebugRenderer::Vertex &v) {
            if (i < 0)
                i += 3; // negative indexing support

            if (i < 0 || i >= VERTEX_COUNT)
                throw nb::index_error("Index out of range");

            t.mV[i] = v;
        }, nb::is_operator())

        .def("__len__", [VERTEX_COUNT](const DebugRenderer::Triangle &t) {
            return VERTEX_COUNT;
        });

    nb::class_<DebugRenderer::LOD>(debugRendererCls, "LOD", "A single level of detail")
        .def(nb::init<>())
        .def_rw("triangle_batch", &DebugRenderer::LOD::mTriangleBatch)
        .def_rw("distance", &DebugRenderer::LOD::mDistance)
        .def("get_batch", [](DebugRenderer::LOD &self){
            const BatchImpl *batch = (BatchImpl *)self.mTriangleBatch.GetPtr() ;
            return batch;
        });

    nb::object debugRendererGeomRefAttr = m.attr("DebugRendererGeometryRef");
    auto debugRendererGeomRefCls = nb::cast<nb::class_<DebugRenderer::GeometryRef>>(debugRendererGeomRefAttr);
    debugRendererGeomRefCls
        .def("get_index", [](DebugRenderer::GeometryRef &self, Vec3Arg inCameraPosition, const AABox &inWorldSpaceBounds, float inLODScaleSq){
            const DebugRenderer::LOD *lod = &self->GetLOD(inCameraPosition, inWorldSpaceBounds, inLODScaleSq);

            const BatchImpl *batch = static_cast<const BatchImpl *>(lod->mTriangleBatch.GetPtr());
            if (!batch)
                throw nb::value_error("batch == nullptr");

            auto objectIndex = batch->geometryIndex;
            return objectIndex;
        }, "camera_position"_a, "world_space_bounds"_a, "lod_scale_sq"_a);

    // TODO: relocate
    m.def("create_index_buffer_from_triangle_array", CreateIndexBufferFromTriangleArray, "triangles"_a, "tolerance"_a = 1e-6f);

    // TODO: relocate
    nb::object vertexArrayAttr = m.attr("DebugRendererVertexArray");
    auto vertexArrayCls = nb::cast<nb::class_<Array<DebugRenderer::Vertex>>>(vertexArrayAttr);
    vertexArrayCls.def("to_numpy", [](Array<DebugRenderer::Vertex> &self){
        constexpr int ARRAY_SIZE = 9;
        using VertexArray = nb::ndarray<nb::numpy, float, nb::shape<-1, ARRAY_SIZE>, nb::device::cpu, nb::c_contig>;

        float *data = new float[self.size() * ARRAY_SIZE];

        nb::capsule owner(data, [](void *p) noexcept {
            delete[] (float *)p;
        });

        for (size_t i = 0; i < self.size(); ++i)
        {
            auto clr = self[i].mColor.GetUInt32();
            data[0 + i * ARRAY_SIZE] = self[i].mPosition.x;
            data[1 + i * ARRAY_SIZE] = self[i].mPosition.y;
            data[2 + i * ARRAY_SIZE] = self[i].mPosition.z;
            data[3 + i * ARRAY_SIZE] = self[i].mNormal.x;
            data[4 + i * ARRAY_SIZE] = self[i].mNormal.y;
            data[5 + i * ARRAY_SIZE] = self[i].mNormal.z;
            data[6 + i * ARRAY_SIZE] = self[i].mUV.x;
            data[7 + i * ARRAY_SIZE] = self[i].mUV.y;
            data[8 + i * ARRAY_SIZE] = *reinterpret_cast<float*>(&clr);
        }

        return VertexArray(data, {self.size(), ARRAY_SIZE}, owner);
    });

    nb::class_<DebugRenderer::Geometry, RefTarget<DebugRenderer::Geometry>>(debugRendererCls, "Geometry",
        nb::intrusive_ptr<DebugRenderer::Geometry>([](DebugRenderer::Geometry *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "A geometry primitive containing triangle batches for various lods")
        .def(nb::init<const AABox &>(), "bounds"_a)
        .def(nb::init<const DebugRenderer::Batch &, const AABox &>(), "batch"_a, "bounds"_a)
        .def("get_lod", &DebugRenderer::Geometry::GetLOD,
            "camera_position"_a, "world_space_bounds"_a, "lod_scale_sq"_a, nb::rv_policy::reference_internal,
            "Determine which LOD to render.\n"
            "Args:\n"
            "    camera_position (Vec3): Current position of the camera.\n"
            "    world_space_bounds (AABox): World space bounds for this geometry (transform mBounds by model space matrix).\n"
            "    lod_scale_sq (float): is the squared scale of the model matrix, it is multiplied with the LOD distances in inGeometry to calculate the real LOD distance (so a number > 1 will force a higher LOD).\n"
            "Returns:\n"
            "    undefined_return: The selected LOD.")
        .def_rw("lods", &DebugRenderer::Geometry::mLODs, "All level of details for this mesh")
        .def_rw("bounds", &DebugRenderer::Geometry::mBounds, "Bounding box that encapsulates all LODs");

    nb::class_<DebugRenderer::Batch>(debugRendererCls, "Batch", "Handle for a batch of triangles");
    debugRendererCls
        .def("draw_triangle", &DebugRenderer::DrawTriangle,
            "v1"_a, "v2"_a, "v3"_a, "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::Off,
            "Draw a single back face culled triangle")

        .def("draw_box", nb::overload_cast<const AABox &, ColorArg, DebugRenderer::ECastShadow, DebugRenderer::EDrawMode>(&DebugRenderer::DrawBox),
            "box"_a, "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a box")

        .def("draw_box", nb::overload_cast<RMat44Arg, const AABox &, ColorArg, DebugRenderer::ECastShadow, DebugRenderer::EDrawMode>(&DebugRenderer::DrawBox),
            "matrix"_a, "box"_a, "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a box")

        .def("draw_sphere", nb::overload_cast<RVec3Arg, float, ColorArg, DebugRenderer::ECastShadow, DebugRenderer::EDrawMode>(&DebugRenderer::DrawSphere),
            "center"_a, "radius"_a, "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a sphere")

        .def("draw_unit_sphere", &DebugRenderer::DrawUnitSphere, "matrix"_a, "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a sphere")

        .def("draw_capsule", &DebugRenderer::DrawCapsule, "matrix"_a, "half_height_of_cylinder"_a, "radius"_a, "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On,
            "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a capsule with one half sphere at (0, -inHalfHeightOfCylinder, 0) and the other half sphere at (0, inHalfHeightOfCylinder, 0) and radius inRadius.\n"
            "The capsule will be transformed by inMatrix.")

        .def("draw_cylinder", &DebugRenderer::DrawCylinder,
            "matrix"_a, "half_height"_a, "radius"_a, "color"_a,
            "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a cylinder with top (0, inHalfHeight, 0) and bottom (0, -inHalfHeight, 0) and radius inRadius.\n"
            "The cylinder will be transformed by inMatrix")

        .def("draw_open_cone", &DebugRenderer::DrawOpenCone, "top"_a,
            "axis"_a, "perpendicular"_a, "half_angle"_a, "length"_a, "color"_a,
            "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a bottomless cone.\n"
            "Args:\n"
            "    top (Vec3): Top of cone, center of base is at inTop + inAxis.\n"
            "    axis (Vec3): Height and direction of cone.\n"
            "    perpendicular (Vec3): Perpendicular vector to inAxis.\n"
            "    half_angle (float): Specifies the cone angle in radians (angle measured between inAxis and cone surface).\n"
            "    length (float): The length of the cone.\n"
            "    color (Color): Color to use for drawing the cone.\n"
            "    cast_shadow (ECastShadow): determines if this geometry should cast a shadow or not.\n"
            "    draw_mode (EDrawMode): determines if we draw the geometry solid or in wireframe.")

        .def("draw_swing_cone_limits", &DebugRenderer::DrawSwingConeLimits,
            "matrix"_a, "swing_y_half_angle"_a, "swing_z_half_angle"_a, "edge_length"_a,
            "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draws cone rotation limits as used by the SwingTwistConstraintPart.\n"
            "Args:\n"
            "    matrix (Mat44): Matrix that transforms from constraint space to world space.\n"
            "    swing_y_half_angle (float): See SwingTwistConstraintPart.\n"
            "    swing_z_half_angle (float): See SwingTwistConstraintPart.\n"
            "    edge_length (float): Size of the edge of the cone shape.\n"
            "    color (Color): Color to use for drawing the cone.\n"
            "    cast_shadow (ECastShadow): determines if this geometry should cast a shadow or not.\n"
            "    draw_mode (EDrawMode): determines if we draw the geometry solid or in wireframe.")

        .def("draw_swing_pyramid_limits", &DebugRenderer::DrawSwingPyramidLimits,
            "matrix"_a, "min_swing_y_angle"_a, "max_swing_y_angle"_a, "min_swing_z_angle"_a, "max_swing_z_angle"_a,
            "edge_length"_a, "color"_a,
            "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draws rotation limits as used by the SwingTwistConstraintPart.\n"
            "Args:\n"
            "    matrix (Mat44): Matrix that transforms from constraint space to world space.\n"
            "    min_swing_y_angle (float): See SwingTwistConstraintPart.\n"
            "    max_swing_y_angle (float): See SwingTwistConstraintPart.\n"
            "    min_swing_z_angle (float): See SwingTwistConstraintPart.\n"
            "    max_swing_z_angle (float): See SwingTwistConstraintPart.\n"
            "    edge_length (float): Size of the edge of the cone shape.\n"
            "    color (Color): Color to use for drawing the cone.\n"
            "    cast_shadow (ECastShadow): determines if this geometry should cast a shadow or not.\n"
            "    draw_mode (EDrawMode): determines if we draw the geometry solid or in wireframe.")

        .def("draw_pie", &DebugRenderer::DrawPie, "center"_a, "radius"_a,
            "normal"_a, "axis"_a, "min_angle"_a, "max_angle"_a, "color"_a,
            "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a pie (part of a circle).\n"
            "Args:\n"
            "    center (Vec3): The center of the circle.\n"
            "    radius (float): Radius of the circle.\n"
            "    normal (Vec3): The plane normal in which the pie resides.\n"
            "    axis (Vec3): The axis that defines an angle of 0 radians.\n"
            "    min_angle (float): The pie will be drawn between [inMinAngle, inMaxAngle] (in radians).\n"
            "    max_angle (float): The pie will be drawn between [inMinAngle, inMaxAngle] (in radians).\n"
            "    color (Color): Color to use for drawing the pie.\n"
            "    cast_shadow (ECastShadow): determines if this geometry should cast a shadow or not.\n"
            "    draw_mode (EDrawMode): determines if we draw the geometry solid or in wireframe.")

        .def("draw_tapered_cylinder", &DebugRenderer::DrawTaperedCylinder,
            "matrix"_a, "top"_a, "bottom"_a, "top_radius"_a, "bottom_radius"_a,
            "color"_a, "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw a tapered cylinder.\n"
            "Args:\n"
            "    matrix (Mat44): Matrix that transforms the cylinder to world space.\n"
            "    top (float): Top of cylinder (along Y axis).\n"
            "    bottom (float): Bottom of cylinder (along Y axis).\n"
            "    top_radius (float): Radius at the top.\n"
            "    bottom_radius (float): Radius at the bottom.\n"
            "    color (Color): Color to use for drawing the pie.\n"
            "    cast_shadow (ECastShadow): determines if this geometry should cast a shadow or not.\n"
            "    draw_mode (EDrawMode): determines if we draw the geometry solid or in wireframe.")

        .def_rw_static("instance", &DebugRenderer::sInstance)

        .def_static("calculate_bounds", &DebugRenderer::sCalculateBounds, "vertices"_a, "vertex_count"_a, "Calculate bounding box for a batch of triangles")

        .def("create_triangle_batch",
             nb::overload_cast<const DebugRenderer::Triangle *, int>(&DebugRenderer::CreateTriangleBatch),
            "triangles"_a, "triangle_count"_a, nb::sig("def create_triangle_batch(self, triangles: DebugRendererTriangleArray) -> BatchImpl"),
            "Create a batch of triangles that can be drawn efficiently")

        .def("create_triangle_batch",
             nb::overload_cast<const DebugRenderer::Vertex *, int, const uint32 *, int>(&DebugRenderer::CreateTriangleBatch),
            "vertices"_a, "vertex_count"_a, "indices"_a, "index_count"_a,
            nb::sig("def create_triangle_batch(self, vertices: DebugRendererVertexArray, indices: numpy.ndarray) -> BatchImpl"))

        .def("create_triangle_batch",
             nb::overload_cast<const Array<DebugRenderer::Triangle> &>(&DebugRenderer::CreateTriangleBatch),
            "triangles"_a)

        .def("create_triangle_batch",
             nb::overload_cast<const Array<DebugRenderer::Vertex> &, const Array<uint32> &>(&DebugRenderer::CreateTriangleBatch),
            "vertices"_a, "indices"_a)

        .def("create_triangle_batch",
             nb::overload_cast<const VertexList &, const IndexedTriangleNoMaterialList &>(&DebugRenderer::CreateTriangleBatch),
            "vertices"_a, "indices"_a)

        .def("create_triangle_batch_for_convex", [](DebugRenderer &self, std::function<Vec3(Vec3Arg)> func, int level, nb::object outBounds_obj) {
            AABox *out_bounds_ptr = nullptr;
            AABox temp_bounds;
            if (!outBounds_obj.is_none()) {
                out_bounds_ptr = &temp_bounds;
            }
            DebugRenderer::Batch batch = self.CreateTriangleBatchForConvex(func, level, out_bounds_ptr);
            if (out_bounds_ptr && !outBounds_obj.is_none()) {
                return nb::make_tuple(batch, temp_bounds);
            }
            return nb::make_tuple(batch, nb::none());
        }, "get_support_func"_a, "level"_a, "out_bounds_obj"_a.none() = nb::none(),
           "Create a primitive for a convex shape using its support function")

        .def("create_triangle_geometry_for_convex", &DebugRenderer::CreateTriangleGeometryForConvex, "get_support_func"_a)

        .def("draw_geometry", nb::overload_cast<RMat44Arg, const AABox &, float, ColorArg, const DebugRenderer::GeometryRef &, DebugRenderer::ECullMode,
                                                DebugRenderer::ECastShadow, DebugRenderer::EDrawMode>(&DebugRenderer::DrawGeometry),
            "model_matrix"_a,"world_space_bounds"_a, "lod_scale_sq"_a, "model_color"_a, "geometry"_a,
            "cull_mode"_a = DebugRenderer::ECullMode::CullBackFace, "cast_shadow"_a = DebugRenderer::ECastShadow::On,
            "draw_mode"_a = DebugRenderer::EDrawMode::Solid,
            "Draw some geometry.\n"
            "Args:\n"
            "    model_matrix (Mat44): is the matrix that transforms the geometry to world space.\n"
            "    world_space_bounds (AABox): is the bounding box of the geometry after transforming it into world space.\n"
            "    lod_scale_sq (float): is the squared scale of the model matrix, it is multiplied with the LOD distances in inGeometry to calculate the real LOD distance (so a number > 1 will force a higher LOD).\n"
            "    model_color (Color): is the color with which to multiply the vertex colors in inGeometry.\n"
            "    geometry (Geometry): The geometry to draw.\n"
            "    cull_mode (ECullMode): determines which polygons are culled.\n"
            "    cast_shadow (ECastShadow): determines if this geometry should cast a shadow or not.\n"
            "    draw_mode (EDrawMode): determines if we draw the geometry solid or in wireframe.")

        .def("draw_geometry", nb::overload_cast<RMat44Arg, ColorArg, const DebugRenderer::GeometryRef &, DebugRenderer::ECullMode,
                                DebugRenderer::ECastShadow, DebugRenderer::EDrawMode>(&DebugRenderer::DrawGeometry),
            "model_matrix"_a, "model_color"_a, "geometry"_a, "cull_mode"_a = DebugRenderer::ECullMode::CullBackFace,
            "cast_shadow"_a = DebugRenderer::ECastShadow::On, "draw_mode"_a = DebugRenderer::EDrawMode::Solid)

        .def("draw_text3d", [](DebugRenderer &self, RVec3Arg position, const char *text, ColorArg color, float height) {
            self.DrawText3D(position, text, color, height);
        }, "position"_a, "text"_a, "color"_a = Color::sWhite, "height"_a = 0.5f, "Draw text");
}
