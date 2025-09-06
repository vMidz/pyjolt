#include "Common.h"
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Core/Core.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Skeleton/Skeleton.h>
#include <Jolt/Physics/Collision/GroupFilter.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/PhysicsScene.h>
#include <nanobind/intrusive/counter.h>
#include <nanobind/intrusive/ref.h>

namespace nanobind {
namespace detail {

template <typename T>
struct type_caster<Ref<T>> {
  public:
    using Value = Ref<T>;

    NB_TYPE_CASTER(Value, make_caster<T>::Name);

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept {

        type_caster<T *> ptr_caster;
        if (!ptr_caster.from_python(src, flags, cleanup))
            return false;

        value = Value(ptr_caster.value);
        return true;
    }

    static handle from_cpp(const Value &ref_value, rv_policy policy, cleanup_list *cleanup) noexcept {
        T *ptr = ref_value.GetPtr();
        if (!ptr)
            return nb::none().release();

        return make_caster<T *>::from_cpp(ptr, policy, cleanup);
    }
};

template <typename T>
struct type_caster<RefConst<T>> {
  public:
    using Value = RefConst<T>;

    NB_TYPE_CASTER(Value, make_caster<T>::Name);

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept {
        type_caster<const T *> ptr_caster;
        if (!ptr_caster.from_python(src, flags, cleanup))
            return false;

        value = Value(ptr_caster.value);
        return true;
    }

    static handle from_cpp(const Value &ref_const_value, rv_policy policy, cleanup_list *cleanup) noexcept {
        const T *ptr = ref_const_value.GetPtr();
        if (!ptr)
            return nb::none().release();

        return make_caster<const T *>::from_cpp(ptr, policy, cleanup);
    }
};

} // namespace detail
} // namespace nanobind

template <typename ResultT>
void BindInst(nb::module_ &m, const char *name) {
    nb::class_<ResultT> result_cls(m, name, "Helper class that either contains a valid result or an error");
    result_cls.def(nb::init<>(), "Default constructor")
        .def(nb::init<const ResultT &>(), "rhs"_a, "Copy constructor")
        .def("assign", [](ResultT &self, ResultT &rhs) {
            self = rhs;
        })
        .def("clear", &ResultT::Clear, "Clear result or error")
        .def("is_empty", &ResultT::IsEmpty, "Checks if the result is still uninitialized")
        .def("is_valid", &ResultT::IsValid, "Checks if the result is valid")
        .def("get", [](const ResultT &self) {
            return self.Get();
        }, nb::rv_policy::reference_internal, "Get the result value")

        // .def("set", [](ResultT &self, T *shape) {
        //     Ref<T> ref(shape);
        //     self.Set(ref);
        // }, "shape"_a, "Set the result value")

        .def("has_error", &ResultT::HasError, "Check if we had an error")
        .def("get_error", [](const ResultT &self) {
            return nb::str(self.GetError().c_str());
        }, "Get the error value")

        .def("set_error", nb::overload_cast<const char *>(&ResultT::SetError), "error_message"_a, "Set an error value")

        .def("__bool__", &ResultT::IsValid);
}

void BindResult(nb::module_ &m) {
    BindInst<Result<Ref<Shape>>>(m, "ShapeResultRef");
    BindInst<Result<Ref<GroupFilter>>>(m, "GroupFilterResultRef");
    BindInst<Result<Ref<PhysicsMaterial>>>(m, "PhysicsMaterialResultRef");
    BindInst<Result<Ref<Skeleton>>>(m, "SkeletonResultRef");
    BindInst<Result<Ref<PhysicsScene>>>(m, "PhysicsSceneResultRef");

    BindInst<Result<BodyCreationSettings>>(m, "BodyCreationSettingsResult");
}