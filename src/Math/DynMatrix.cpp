#include "Common.h"
#include <Jolt/Math/DynMatrix.h>

void BindDynMatrix(nb::module_ &m) {
    nb::class_<DynMatrix>(m, "DynMatrix", "Dynamic resizable matrix class")
        .def(nb::init<const DynMatrix&>(), "matrix"_a)
        .def(nb::init<uint, uint>(), "rows"_a, "cols"_a, "Constructor")
        .def_prop_ro("rows", &DynMatrix::GetRows)
        .def_prop_ro("cols", &DynMatrix::GetCols, "Get dimensions")

        .def("__getitem__", [](const DynMatrix &mat, nb::typed<nb::tuple, int, int> &index) {
            auto first = nb::cast<int>(index[0]);
            auto second = nb::cast<int>(index[1]);
            if (first >= mat.GetRows() || second >= mat.GetCols())
                throw nb::index_error("Matrix index out of bounds");

            return mat(first, second);
        }, "index"_a, "Access element at (row, col). Use tuple index: mat[row, col].")
        .def("__setitem__", [](DynMatrix &mat, nb::typed<nb::tuple, int, int> &index, float value) {
            auto first = nb::cast<int>(index[0]);
            auto second = nb::cast<int>(index[1]);
            if (first >= mat.GetRows() || second >= mat.GetCols())
                throw nb::index_error("Matrix index out of bounds");

            mat(first, second) = value;
        }, "index"_a, "value"_a, "Set element at (row, col). Use tuple index: mat[row, col] = value.")

        .def("__repr__", [](const DynMatrix &self) {
            std::ostringstream oss;
            oss << "DynMatrix(rows= " << self.GetRows() << ", cols= " << self.GetCols() << ")\n[";
            for (size_t r = 0; r < self.GetCols(); ++r) {
                for (size_t c = 0; c < self.GetRows(); ++c) {
                    oss << self(r, c);
                    if (c + 1 < self.GetRows())
                        oss << ", ";
                }

                if (r + 1 < self.GetCols())
                    oss << '\n';
            }
            oss << "]";
            return nb::str(oss.str().c_str());
        }, "Return a string representation showing dimensions.");
}