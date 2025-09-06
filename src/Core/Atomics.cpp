#include "Common.h"
#include <Jolt/Core/Atomics.h>
#include <atomic>
#include <string>

template <typename T>
void BindAtomic(nb::module_ &m, std::string name) {
    nb::class_<atomic<T>>(m, (name + "atomic").c_str());
    m.def(("atomic_min_" + name).c_str(), &AtomicMin<T>, "atomic"_a, "value"_a, "memory_order"_a = (int)std::memory_order_seq_cst,
        "Atomically compute the min(ioAtomic, inValue) and store it in ioAtomic, returns true if value was updated");
    m.def(("atomic_max_" + name).c_str(), &AtomicMax<T>, "atomic"_a, "value"_a, "memory_order"_a = (int)std::memory_order_seq_cst,
        "Atomically compute the max(ioAtomic, inValue) and store it in ioAtomic, returns true if value was updated");
}

void BindAtomics(nb::module_ &m) {
    nb::enum_<memory_order>(m, "memory_order")
        .value("MEMORY_ORDER_RELAXED", memory_order::memory_order_relaxed)
        .value("MEMORY_ORDER_CONSUME", memory_order::memory_order_consume)
        .value("MEMORY_ORDER_ACQUIRE", memory_order::memory_order_acquire)
        .value("MEMORY_ORDER_RELEASE", memory_order::memory_order_release)
        .value("MEMORY_ORDER_ACQ_REL", memory_order::memory_order_acq_rel);

    BindAtomic<int>(m, "int");
    BindAtomic<float>(m, "float");
}