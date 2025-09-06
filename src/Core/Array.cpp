#include "Common.h"
#include <Jolt/AABBTree/AABBTreeBuilder.h>
#include <Jolt/Core/Array.h>
#include <Jolt/Core/LinearCurve.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Physics/Character/Character.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/SubShapeID.h>
#include <Jolt/Physics/Collision/Shape/CompoundShape.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyVertex.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Ragdoll/Ragdoll.h>
#include <Jolt/Physics/Vehicle/VehicleAntiRollBar.h>
#include <Jolt/Physics/Vehicle/VehicleDifferential.h>
#include <Jolt/Physics/Vehicle/Wheel.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/BroadPhase/QuadTree.h>
#include <Jolt/Geometry/AABox.h>
#include <Jolt/Geometry/Plane.h>
#include <Jolt/Geometry/ConvexHullBuilder.h>
#include <Jolt/Skeleton/SkeletalAnimation.h>
#include <Jolt/Skeleton/Skeleton.h>
#include <Jolt/Skeleton/SkeletonMapper.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Renderer/DebugRendererRecorder.h>
#include <Jolt/TriangleGrouper/TriangleGrouper.h>
#include <Jolt/TriangleSplitter/TriangleSplitter.h>
#include <Jolt/ObjectStream/TypeDeclarations.h>

#include <nanobind/operators.h>
#include <nanobind/make_iterator.h>

// Helper for SFINAE to detect operator== for an element type U
template <typename U, typename = std::void_t<>>
struct has_element_operator_equals_equals : std::false_type {};

template <typename U>
struct has_element_operator_equals_equals<U, std::void_t<decltype(std::declval<const U &>() == std::declval<const U &>())>> : std::true_type {};

template <typename U>
inline constexpr bool has_element_operator_equals_equals_v = has_element_operator_equals_equals<U>::value;

// Helper for SFINAE to detect operator!= for an element type U
template <typename U, typename = std::void_t<>>
struct has_element_operator_not_equals : std::false_type {};

template <typename U>
struct has_element_operator_not_equals<U, std::void_t<decltype(std::declval<const U &>() != std::declval<const U &>())>> : std::true_type {};

template <typename U>
inline constexpr bool has_element_operator_not_equals_v = has_element_operator_not_equals<U>::value;

template <typename T, bool BindComparisonOp = true, bool BindConstructor = true, bool BindSetter = true>
nb::class_<Array<T>> BindInst(nb::module_ &m, std::string name) {
    using ArrayT = Array<T>;
    using Allocator = STLAllocator<T>;
    using const_iterator = const T *;
    nb::class_<Allocator>(m, (name + "Allocator").c_str(), "Class that allocates tree nodes, can be shared between multiple trees");
    nb::class_<ArrayT> cls(m, (name + "Array").c_str());
    cls
        .def(nb::init<>(), "Default constructor")
        .def(nb::init<const Allocator &>(),
             "allocator"_a,
             "Constructor with allocator")
        .def(nb::init<size_t, const Allocator &>(),
             "length"_a, "allocator"_a,
             "Constructor with length")
        .def(nb::init<ArrayT>(), "allocator"_a, "Constructor with allocator")
        .def(nb::init<size_t, const T &, const Allocator &>(),
             "length"_a, "value"_a, "allocator"_a,
             "Constructor with length and value");

    if constexpr (BindConstructor) {
        cls.def(nb::init<const_iterator, const_iterator, const Allocator>(),
                 "begin"_a, "end"_a, "allocator"_a,
                 "Constructor from iterator");
        }

     cls.def(nb::init<const Array<T, Allocator> &>(), "rhs"_a, "Copy constructor")
        .def(nb::init<const Array<T, Allocator> &&>(), "rhs"_a, "Move constructor")

        .def("reserve", &ArrayT::reserve, "new_size"_a, "Reserve array space")
        .def("resize", nb::overload_cast<size_t>(&ArrayT::resize), "new_size"_a, "Destruct all elements and set length to zero")
        .def("clear", &ArrayT::clear, "Destruct all elements and set length to zero")

        .def("assign", [](ArrayT &self, ArrayT &other, uint begin_idx, uint end_idx) {
            self.assign(other.begin() + begin_idx, other.begin() + end_idx);
        }, "other"_a, "begin_index"_a, "end_index"_a, "Replace the contents of this array with inList")
        .def("get_allocator", nb::overload_cast<>(&ArrayT::get_allocator), "Get the allocator")
        .def("push_back", nb::overload_cast<const T &>(&ArrayT::push_back), "value"_a, "Add element to the back of the array")
        .def("emplace_back", &ArrayT::template emplace_back<T>, "value"_a, "Construct element at the back of the array")
        .def("pop_back", &ArrayT::pop_back, "Remove element from the back of the array")
        .def("empty", &ArrayT::empty, "Returns true if there are no elements in the array")
        .def("size", &ArrayT::size, "Returns amount of elements in the array")
        .def("capacity", &ArrayT::capacity, "Returns maximum amount of elements the array can hold")
        .def("shrink_to_fit", &ArrayT::shrink_to_fit, "Reduce the capacity of the array to match its size")
        .def("swap", &ArrayT::swap, "rhs"_a, "Swap the contents of two arrays")
        .def("insert", [](ArrayT &arr, size_t index, const T &value) {
            if (index > arr.size())
                throw nb::index_error("Index out of range");
            arr.insert(arr.begin() + index, value);
        }, "index"_a, "value"_a)
        .def("erase", [](ArrayT &arr, size_t index) {
            if (index >= arr.size())
                throw nb::index_error("Index out of range");
            arr.erase(arr.begin() + index);
        }, "index"_a, "Remove one element from the array")

        .def("begin", nb::overload_cast<>(&ArrayT::begin))
        .def("end", nb::overload_cast<>(&ArrayT::end))
        .def("data", nb::overload_cast<>(&ArrayT::data))
        .def("at", nb::overload_cast<size_t>(&ArrayT::at), "index"_a)
        .def("front", nb::overload_cast<>(&ArrayT::front), "First element in the array")
        .def("back", nb::overload_cast<>(&ArrayT::back), "Last element in the array")

        // Operators
        .def("__getitem__", [](const ArrayT &arr, int i) {
            if (i >= arr.size())
                throw nb::index_error("Index out of range");
            return arr[i];
        }, "index"_a);

    if constexpr (BindSetter) {
        cls.def("__setitem__", [](ArrayT &arr, size_t i, const T &v) {
            if (i >= arr.size())
                throw nb::index_error("Index out of range");
            arr[i] = v;
        }, "index"_a, "value"_a);
    }
    if constexpr (has_element_operator_equals_equals_v<T> && BindComparisonOp) {
        cls.def(nb::self == nb::self, "rhs"_a);
    }
    if constexpr (has_element_operator_not_equals_v<T> && BindComparisonOp) {
        cls.def(nb::self != nb::self, "rhs"_a);
    }

    // Python-specific
    cls
        .def("__len__", &ArrayT::size)
        .def("__bool__", [](const ArrayT &v) {
            return !v.empty();
        })
        .def("__iter__", [](const ArrayT &self) {
            return nb::make_iterator(nb::type<ArrayT>(), "iterator", self.begin(), self.end());
        })
        .def("erase_range", [](ArrayT &arr, size_t begin, size_t end) {
            if (begin > arr.size() || end > arr.size() || begin > end)
                throw nb::index_error("Invalid range");
            arr.erase(arr.begin() + begin, arr.begin() + end);
        }, "begin"_a, "end"_a)
        .def("to_list", [](const ArrayT &a) {
            nb::list result;
            for (const auto &item : a)
                result.append(item);
            return result;
        })
        .def("__repr__", [](const ArrayT &self) {
            nb::str str = nb::str("Array[");
            for (size_t i = 0; i < self.size(); ++i) {
                auto pyObj = nb::find(self[i]);
                if (pyObj.is_valid())
                    str += nb::repr(pyObj);
                else
                    break;
                if (i + 1 < self.size())
                    str += nb::str(", ");
            }
            str += nb::str("]");
            return str;
        });

    return cls;
}

void BindArray(nb::module_ &m) {
    // Primitve types
    auto cls = BindInst<uint8>(m, "Uint8");
    cls.doc() = "Simple replacement for std::vector\n"
                "Major differences:\n"
                "- Memory is not initialized to zero (this was causing a lot of page faults when deserializing large MeshShapes / HeightFieldShapes)\n"
                "- Iterators are simple pointers (for now)\n"
                "- No exception safety\n"
                "- No specialization like std::vector<bool> has\n"
                "- Not all functions have been implemented";

    BindInst<uint16>(m, "Uint16");

    BindInst<uint32>(m, "Uint32");
    BindInst<int32_t>(m, "Int32T");

    BindInst<float>(m, "Float");
    BindInst<double>(m, "Double");

    // Jolt math types
    BindInst<Float2>(m, "Float2");
    BindInst<Float3>(m, "Float3");
    BindInst<Float4>(m, "Float4");

    BindInst<Mat44>(m, "Mat44");

    BindInst<Vec3>(m, "Vec3");
    BindInst<Vec4>(m, "Vec4");

    BindInst<DVec3>(m, "DVec3");
    BindInst<DMat44>(m, "DMat44");

    BindInst<nb::str>(m, "PyStr");     // TODO: Remove?

    // Jolt types
    BindInst<LinearCurve::Point>(m, "LinearCurvePoint");

    BindInst<SkeletalAnimation::Keyframe>(m, "SkeletalAnimationKeyframe");
    BindInst<SkeletalAnimation::AnimatedJoint>(m, "SkeletalAnimationAnimatedJoint");
    BindInst<SkeletalAnimation::JointState>(m, "SkeletalAnimationJointState");

    BindInst<Skeleton::Joint>(m, "SkeletonJoint");

    BindInst<SkeletonMapper::Chain>(m, "SkeletonMapperChain");
    BindInst<SkeletonMapper::Mapping>(m, "SkeletonMapperMapping");
    BindInst<SkeletonMapper::Unmapped>(m, "SkeletonMapperUnmapped");
    BindInst<SkeletonMapper::Locked>(m, "SkeletonMapperLocked");

    BindInst<CharacterVirtual::Contact>(m, "CharacterVirtualContact");

    BindInst<CompoundShape::SubShape>(m, "CompoundShapeSubShape");

    BindInst<CompoundShapeSettings::SubShapeSettings>(m, "CompoundShapeSettingsSubShapeSettings");

    BindInst<Triangle>(m, "Triangle");

    BindInst<BodyID>(m, "BodyID");

    BindInst<IndexedTriangleList, false>(m, "IndexedTriangleList");  // Type Array<IndexedTriangle>;

    BindInst<IndexedTriangle>(m, "IndexedTriangle");
    BindInst<PhysicsMaterialRefC>(m, "PhysicsMaterialList");

    BindInst<SoftBodySharedSettings::Face>(m, "SoftBodySharedSettingsFace");
    BindInst<SoftBodySharedSettings::Edge>(m, "SoftBodySharedSettingsEdge");
    BindInst<SoftBodySharedSettings::Vertex>(m, "SoftBodySharedSettingsVertex");
    BindInst<SoftBodySharedSettings::DihedralBend>(m, "SoftBodySharedSettingsDihedralBend");
    BindInst<SoftBodySharedSettings::Volume>(m, "SoftBodySharedSettingsVolume");
    BindInst<SoftBodySharedSettings::Skinned>(m, "SoftBodySharedSettingsSkinned");
    BindInst<SoftBodySharedSettings::InvBind>(m, "SoftBodySharedSettingsInvBind");
    BindInst<SoftBodySharedSettings::LRA>(m, "SoftBodySharedSettingsLRA");
    BindInst<SoftBodyVertex>(m, "SoftBodyVertex");

    BindInst<Ref<Constraint>>(m, "RefConstraint");
    BindInst<Ref<SoftBodySharedSettings>>(m, "RefSoftBodySharedSettings");
    BindInst<Ref<PhysicsMaterial>>(m, "RefPhysicsMaterial");
    BindInst<Ref<GroupFilter>>(m, "RefGroupFilter");

    BindInst<Plane>(m, "Plane");

    BindInst<PhysicsScene::ConnectedConstraint>(m, "PhysicsSceneConnectedConstraint");
    BindInst<SoftBodyCreationSettings>(m, "SoftBodyCreationSettings");

    BindInst<BodyCreationSettings>(m, "BodyCreationSettings");
    BindInst<RagdollSettings>(m, "RagdollSettings");
    BindInst<RagdollSettings::Part>(m, "RagdollSettingsPart");
    BindInst<RagdollSettings::AdditionalConstraint>(m, "RagdollSettingsAdditionalConstraint");

    BindInst<DebugRenderer::LOD>(m, "DebugRendererLOD");
    BindInst<DebugRenderer::Triangle>(m, "DebugRendererTriangle");
    BindInst<DebugRenderer::Vertex>(m, "DebugRendererVertex");
    BindInst<IndexedTriangleNoMaterial>(m, "IndexedTriangleNoMaterial");

    // BindInst<Shape>(m, "Shape");
    BindInst<Ref<Shape>>(m, "RefShape");
    BindInst<RefConst<Shape>>(m, "RefConstShape");

    BindInst<VehicleAntiRollBar>(m, "VehicleAntiRollBar");
    BindInst<VehicleDifferentialSettings>(m, "VehicleDifferentialSettings");
    BindInst<Ref<WheelSettings>>(m, "RefWheelSettings");

    BindInst<DebugRendererRecorder::LineBlob>(m, "DebugRendererRecorderLineBlob");
    BindInst<DebugRendererRecorder::TriangleBlob>(m, "DebugRendererRecorderTriangleBlob");
    BindInst<DebugRendererRecorder::TextBlob>(m, "DebugRendererRecorderTextBlob");
    BindInst<DebugRendererRecorder::GeometryBlob>(m, "DebugRendererRecorderGeometryBlob");

    BindInst<QuadTree::Tracking, true, true, false>(m, "QuadTreeTracking");

    BindInst<ShapeCastResult, false, false>(m, "ShapeCastResult");

    // TODO:
            BindInst<Body *, false, false>(m, "ConvexHullBuilderFaceP");
            BindInst<ConvexHullBuilder::Face *, false, false>(m, "ConvexHullBuilderFaceP");
            BindInst<CharacterVirtual *, false, false>(m, "CharacterVirtualP");
            BindInst<const AABBTreeBuilder::Node *, false, false>(m, "AABBTreeBuilderNodeP");
}