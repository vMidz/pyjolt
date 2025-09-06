#include "Common.h"
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Physics/Constraints/TwoBodyConstraint.h>
#include <Jolt/Physics/Constraints/PathConstraint.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Collision/Shape/HeightFieldShape.h>
#include <Jolt/Physics/Collision/Shape/MutableCompoundShape.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>
#include <Jolt/Physics/Ragdoll/Ragdoll.h>
#include <Jolt/Physics/Character/CharacterBase.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/VehicleController.h>
#include <Jolt/Physics/Vehicle/Wheel.h>
#include <Jolt/Skeleton/Skeleton.h>
#include <Jolt/Skeleton/SkeletonMapper.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include <nanobind/operators.h>
#include <nanobind/intrusive/counter.inl>

template <typename T>
void BindRef(nb::module_ &m, std::string className) {
    using CurrentRef = Ref<T>;

    nb::class_<CurrentRef>(m, (className + "Ref").c_str(),
        "Reference counting smart pointer (like shared_ptr). Manages lifetime of RefTarget objects.")
        // Constructors
        .def(nb::init<>(), "Default constructor (nullptr)")
        .def(nb::init<T *>(), "rhs"_a)
        .def(nb::init<const CurrentRef &>(), "rhs"_a)

        // Python doesn't have "=" operator
        .def("assign", [](CurrentRef &self, T *ptr) -> CurrentRef & {
            self = ptr;
            return self;
        }, "rhs"_a, "Assign from raw pointer T*")
        .def("assign", [](CurrentRef &self, const CurrentRef &other) -> CurrentRef & {
            self = other;
            return self;
        }, "rhs"_a, "Assignment operators")
        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self == static_cast<const T *>(nullptr), "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")
        .def(nb::self != static_cast<const T *>(nullptr), "rhs"_a, "Comparison")

        .def("get", &CurrentRef::GetPtr, "Get pointer")
        .def("internal_get_pointer", &CurrentRef::InternalGetPointer, "INTERNAL HELPER FUNCTION USED BY SERIALIZATION");
}

template <typename T>
void BindRefConst(nb::module_ &m, std::string className) {
    using CurrentRefConst = RefConst<T>;

    nb::class_<CurrentRefConst>(m, (className + "RefConst").c_str(),
        "Reference counting const smart pointer (like shared_ptr<const T>). Manages lifetime of RefTarget objects.")
        // Constructors
        .def(nb::init<>(), "Default constructor (nullptr)")
        .def(nb::init<const T *>(), "rhs"_a, "Constructor from raw pointer const T*")
        .def(nb::init<const Ref<T> &>(), "rhs"_a, "Constructor from Ref<T>")
        .def("assign", [](CurrentRefConst &self, const T *ptr) -> CurrentRefConst & {
            self = ptr;
            return self;
        }, "rhs"_a, "Assignment operators")
        .def("assign", [](CurrentRefConst &self, const CurrentRefConst &other) -> CurrentRefConst & {
            self = other;
            return self;
        }, "rhs"_a, "Assignment operators")
        .def("assign", [](CurrentRefConst &self, const Ref<T> &other) -> CurrentRefConst & {
            self = other;
            return self;
        }, "rhs"_a, "Assignment operators")

        .def(nb::self == nb::self, "rhs"_a)
        .def(nb::self == (const T *)nullptr, "rhs"_a)
        .def(nb::self == (const Ref<T> )nullptr, "rhs"_a)

        .def(nb::self != nb::self, "rhs"_a)
        .def(nb::self != (const T *)nullptr, "rhs"_a)
        .def(nb::self != (const Ref<T> )nullptr, "rhs"_a)

        .def("get_ptr", &CurrentRefConst::GetPtr, "Get pointer")
        .def("internal_get_pointer", &CurrentRefConst::InternalGetPointer, "INTERNAL HELPER FUNCTION USED BY SERIALIZATION");
}

template <typename T>
void BindRefTarget(nb::module_ &m, std::string className) {
    nb::class_<RefTarget<T>>(m, (className + "RefTarget").c_str(),
        "Base class for reference counting (template).")
        .def(nb::init<>(), "Default constructor")
        .def("get_ref_count", &RefTarget<T>::GetRefCount, "Get current refcount of this object")
        .def("set_embedded", &RefTarget<T>::SetEmbedded,
            "Mark this class as embedded, this means the type can be used in a compound or constructed on the stack.\n"
            "The Release function will never destruct the object, it is assumed the destructor will be called by whoever allocated\n"
            "the object and at that point in time it is checked that no references are left to the structure.")
        .def("add_ref", &RefTarget<T>::AddRef, "Add or release a reference to this object")
        .def("release", &RefTarget<T>::Release)
        .def_static("internal_get_ref_count_offset", &RefTarget<T>::sInternalGetRefCountOffset, "INTERNAL HELPER FUNCTION USED BY SERIALIZATION");
}

template <typename T>
void BindRefTargetVirtual(nb::module_ &m, std::string className) {
    nb::class_<RefTargetVirtual<T>>(m, (className + "RefTargetVirtual").c_str(), "Pure virtual version of RefTarget")
        .def("add_ref", &RefTargetVirtual::AddRef, "Virtual add reference")
        .def("release", &RefTargetVirtual::Release, "Virtual release reference");
}

void BindReference(nb::module_ &m) {

    nb::intrusive_init(
        [](PyObject *o) noexcept {
            nb::gil_scoped_acquire guard;
            Py_INCREF(o);
        },
        [](PyObject *o) noexcept {
            nb::gil_scoped_acquire guard;
            Py_DECREF(o);
        });

    BindRefTarget<PhysicsScene>(m, "PhysicsScene");
    BindRefTarget<CharacterBaseSettings>(m, "CharacterBaseSettings");
    BindRefTarget<CharacterBase>(m, "CharacterBase");
    BindRefTarget<GroupFilter>(m, "GroupFilter");
    BindRefTarget<PhysicsMaterial>(m, "PhysicsMaterial");
    BindRefTarget<ShapeSettings>(m, "ShapeSettings");
    BindRefTarget<Shape>(m, "Shape");
    BindRefTarget<ConstraintSettings>(m, "ConstraintSettings");
    BindRefTarget<Constraint>(m, "Constraint");
    BindRefTarget<PathConstraintPath>(m, "PathConstraintPath");
    BindRefTarget<RagdollSettings>(m, "RagdollSettings");
    BindRefTarget<Ragdoll>(m, "Ragdoll");
    BindRefTarget<SoftBodySharedSettings>(m, "SoftBodySharedSettings");
    BindRefTarget<SkeletalAnimation>(m, "SkeletalAnimation");
    BindRefTarget<Skeleton>(m, "Skeleton");
    BindRefTarget<SkeletonMapper>(m, "SkeletonMapper");
    BindRefTarget<DebugRenderer::Geometry>(m, "Geometry");
    BindRefTarget<WheelSettings>(m, "WheelSettings");
    BindRefTarget<VehicleCollisionTester>(m, "VehicleCollisionTester");
    BindRefTarget<VehicleControllerSettings>(m, "VehicleControllerSettings");
    BindRefTarget<VehicleController>(m, "VehicleController");

    BindRef<ConstraintSettings>(m, "ConstraintSettings");
    BindRef<SoftBodySharedSettings>(m, "SoftBodySharedSettings");
    BindRef<Constraint>(m, "Constraint");
    BindRef<GroupFilter>(m, "GroupFilter");
    BindRef<HeightFieldShape>(m, "HeightFieldShape");
    BindRef<MutableCompoundShape>(m, "MutableCompoundShape");
    BindRef<PhysicsMaterial>(m, "PhysicsMaterial");
    BindRef<RagdollSettings>(m, "RagdollSettings");
    BindRef<TwoBodyConstraintSettings>(m, "TwoBodyConstraintSettings");
    BindRef<Shape>(m, "Shape");
    BindRef<ShapeSettings>(m, "ShapeSettings");
    BindRef<Skeleton>(m, "Skeleton");
    BindRef<PathConstraintPath>(m, "PathConstraintPath");
    BindRef<DebugRenderer::Geometry>(m, "DebugRendererGeometry");
    BindRef<WheelSettings>(m, "WheelSettings");
    BindRef<VehicleControllerSettings>(m, "VehicleControllerSettings");

    BindRefConst<Shape>(m, "Shape");
    BindRefConst<ShapeSettings>(m, "ShapeSettings");
    BindRefConst<PhysicsMaterial>(m, "PhysicsMaterial");
    BindRefConst<PathConstraintPath>(m, "PathConstraintPath");
    BindRefConst<TwoBodyConstraintSettings>(m, "TwoBodyConstraintSettings");
    BindRefConst<SoftBodySharedSettings>(m, "SoftBodySharedSettings");
}