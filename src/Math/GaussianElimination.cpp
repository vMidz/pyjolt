#include "Common.h"
#include "BindingUtility/Tuple.h"
#include <Jolt/Math/Matrix.h>
#include <Jolt/Math/GaussianElimination.h>

template <int M1_Rows = 4, int M1_Cols = 4, int M2_Rows = 4, int M2_Cols = 4>
nb::module_ BindInst(nb::module_ &m, const char *name, const char * doc = "") {
    using Matrix1 = Matrix<M1_Rows, M1_Cols>;
    using Matrix2 = Matrix<M2_Rows, M2_Cols>;

    auto func = m.def(name, [](Matrix1 &matrixA, Matrix2 &matrixB, float inTolerance = 1.0e-16f) {
        auto r = GaussianElimination<Matrix1, Matrix2>(matrixA, matrixB, inTolerance);
        return CreateTuple(r, matrixA, matrixB);
    }, "matrix_a"_a, "matrix_b"_a, "tolerance"_a = 1.0e-16f, doc);

    return func;
}

void BindGaussianElimination(nb::module_ &m) {
    auto doc = "This function performs Gauss-Jordan elimination to solve a matrix equation.\n"
              "A must be an NxN matrix and B must be an NxM matrix forming the equation A * x = B\n"
              "on output B will contain x and A will be destroyed.\n"
              "This code can be used for example to compute the inverse of a matrix.\n"
              "Set A to the matrix to invert, set B to identity and let GaussianElimination solve\n"
              "the equation, on return B will be the inverse of A. And A is destroyed.\n"
              "Taken and adapted from Numerical Recipies in C paragraph 2.1\n"
              "Returns:\n"
              "    tuple:\n"
              "    - bool: Results\n"
              "    - Matrix1: MatrixA\n"
              "    - Matrix2: MatrixB\n";

    BindInst<4, 4, 4, 4>(m, "gaussian_elimination_4X4_4X4", doc);
    BindInst<3, 3, 3, 3>(m, "gaussian_elimination_3X3_3X3");
}
