#include "Common.h"

void BindMemory(nb::module_ &m) {
    m.def("allocate", &Allocate, "size"_a, "Directly define the allocation functions");
    m.def("reallocate", &Reallocate, "block"_a, "old_size"_a, "new_size"_a);
    m.def("free", &Free, "block"_a);
    m.def("aligned_free", &AlignedFree, "block"_a);
    m.def("aligned_allocate", &AlignedAllocate, "size"_a, "alignment"_a);
    m.def("register_default_allocator", &RegisterDefaultAllocator, "Don't implement allocator registering");
}