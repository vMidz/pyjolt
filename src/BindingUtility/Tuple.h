#pragma once
#include <nanobind/nanobind.h>

namespace nb = nanobind;

template<typename... Args>
auto CreateTuple(Args&&... args) -> nb::typed<nb::tuple, Args...> {
    return nb::typed<nb::tuple, Args...>(nb::make_tuple(std::forward<Args>(args)...));
}