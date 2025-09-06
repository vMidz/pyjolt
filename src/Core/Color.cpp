#include "Common.h"
#include <nanobind/stl/pair.h>
#include <nanobind/stl/string.h>
#include <nanobind/operators.h>
#include <Jolt/Core/Color.h>

void BindColor(nb::module_ &m) {
    nb::class_<Color>(m, "Color", "Class that holds an RGBA color with 8-bits per component")
        // Constructors
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<uint32_t>(), "color"_a)
        .def(nb::init<uint8_t, uint8_t, uint8_t, uint8_t>(), "r"_a, "g"_a, "b"_a, "a"_a = 255)
        .def(nb::init<const Color &, uint8_t>(), "color"_a, "alpha"_a)

        .def_rw("r", &Color::r)
        .def_rw("g", &Color::g)
        .def_rw("b", &Color::b)
        .def_rw("a", &Color::a)

        .def_static("get_distinct_color", &Color::sGetDistinctColor, "index"_a)

        .def("get_uint32", &Color::GetUInt32, "Convert to uint32")
        .def("to_vec4", &Color::ToVec4, "Convert to Vec4 with range [0, 1]")
        .def("get_intensity", &Color::GetIntensity, "Get grayscale intensity of colo")

        .def_ro_static("Black", &Color::sBlack)
        .def_ro_static("DarkRed", &Color::sDarkRed)
        .def_ro_static("Red", &Color::sRed)
        .def_ro_static("DarkGreen", &Color::sDarkGreen)
        .def_ro_static("Green", &Color::sGreen)
        .def_ro_static("DarkBlue", &Color::sDarkBlue)
        .def_ro_static("Blue", &Color::sBlue)
        .def_ro_static("Yellow", &Color::sYellow)
        .def_ro_static("Purple", &Color::sPurple)
        .def_ro_static("Cyan", &Color::sCyan)
        .def_ro_static("Orange", &Color::sOrange)
        .def_ro_static("DarkOrange", &Color::sDarkOrange)
        .def_ro_static("Grey", &Color::sGrey)
        .def_ro_static("LightGrey", &Color::sLightGrey)
        .def_ro_static("White", &Color::sWhite)

        // Operators
        .def("__getitem__", [](const Color &self, int index) {
            return self(index);
        }, "index"_a, "Element access, 0 = red, 1 = green, 2 = blue, 3 = alpha")
        .def("__setitem__", [](Color &self, int index, uint8 value) {
            self(index) = value;
        }, "index"_a, "value"_a, "Element access, 0 = red, 1 = green, 2 = blue, 3 = alpha")

        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a)
        .def(nb::self * nb::self, "Multiply two colors")
        .def(nb::self * float(), "rhs"_a, "Multiply color with intensity in the range [0, 1]")

        // String representation
        .def("__repr__", [](const Color &self) {
            return nb::str("Color[r={}, g={}, b={}, a={}]").format(self.r, self.g, self.b, self.a);
        });
}