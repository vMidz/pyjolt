#include "Common.h"
#include <nanobind/make_iterator.h>
#include <cstdint>

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/Memory.h>

void BindTempAllocator(nb::module_ &m) {
    nb::class_<TempAllocator, NonCopyable>(m, "TempAllocator",
        "Allocator for temporary allocations.\n"
        "This allocator works as a stack: The blocks must always be freed in the reverse order as they are allocated.\n"
        "Note that allocations and frees can take place from different threads, but the order is guaranteed though\n"
        "job dependencies, so it is not needed to use any form of locking.")

        .def("allocate", [](TempAllocator &self, uint inSize) -> uintptr_t {
            void *ptr = self.Allocate(inSize);
            return reinterpret_cast<uintptr_t>(ptr);
        }, "size"_a, "Allocates inSize bytes of memory, returned memory address must be JPH_RVECTOR_ALIGNMENT byte aligned")
        .def("Free", [](TempAllocator &self, uintptr_t inAddress, uint inSize) {
            self.Free(reinterpret_cast<void *>(inAddress), inSize);
        }, "address"_a, "size"_a, "Frees inSize bytes of memory located at address");

    nb::class_<TempAllocatorImpl, TempAllocator>(m, "TempAllocatorImpl", "Default implementation of the temp allocator that allocates a large block through malloc upfront")
        .def(nb::init<uint>(), "size"_a, "Constructs the allocator with a fixed buffer size.")
        .def("allocate", &TempAllocator::Allocate, "size"_a, "Allocates inSize bytes of memory, returned memory address must be JPH_RVECTOR_ALIGNMENT byte aligned")
        .def("free", &TempAllocatorImpl::Free, "address"_a, "size"_a)
        .def("is_empty", &TempAllocatorImpl::IsEmpty, "Check if no allocations have been made.")
        .def("get_size", &TempAllocatorImpl::GetSize, "Get the total size of the fixed buffer")
        .def("get_usage", &TempAllocatorImpl::GetUsage, "Get current usage in bytes of the buffer")
        .def("can_allocate", &TempAllocatorImpl::CanAllocate, "size"_a, "Check if an allocation of inSize can be made in this fixed buffer allocator")
        .def("owns_memory", [](const TempAllocatorImpl &self, uintptr_t inAddress) {
            return self.OwnsMemory(reinterpret_cast<const void *>(inAddress));
        }, "address"_a, "Check if memory block at inAddress is owned by this allocator");

    nb::class_<TempAllocatorMalloc, TempAllocator>(m, "TempAllocatorMalloc",
        "Implementation of the TempAllocator that just falls back to malloc/free\n"
        "Note: This can be quite slow when running in the debugger as large memory blocks need to be initialized with 0xcd")
        .def("allocate", &TempAllocatorMalloc::Allocate, "size"_a)
        .def("free", &TempAllocatorMalloc::Free, "address"_a, "size"_a);

    nb::class_<TempAllocatorImplWithMallocFallback, TempAllocator>(m, "TempAllocatorImplWithMallocFallback", "Implementation of the TempAllocator that tries to allocate from a large preallocated block, but falls back to malloc when it is exhausted")
        .def(nb::init<uint>(), "size"_a, "Constructs the allocator with an initial fixed block if size")
        .def("allocate", &TempAllocatorImplWithMallocFallback::Allocate, "size"_a)
        .def("free", &TempAllocatorImplWithMallocFallback::Free, "address"_a, "size"_a);
}