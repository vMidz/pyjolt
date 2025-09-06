#include "Common.h"
#include <Jolt/Core/NonCopyable.h>

void BindNonCopyable(nb::module_ &m) {
    nb::class_<NonCopyable>(m, "NonCopyable", "Class that makes another class non-copyable. Usage: Inherit from NonCopyable.");
}