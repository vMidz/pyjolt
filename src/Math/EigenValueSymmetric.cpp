#include "Common.h"
#include "BindingUtility/Tuple.h"
#include <Jolt/Math/EigenValueSymmetric.h>
#include <Jolt/Math/Matrix.h>

template <int Rows = 4, int Cols = 4, int VectorCols = 4>
void BindInst(nb::module_ &m, const char *name, const char* doc = "") {
    auto r = m.def(name, [](Matrix<Rows, Cols> inMatrix) {
        Matrix<Rows, Cols> outEigVec;
        Vector<VectorCols> outEigVal;
        auto r = EigenValueSymmetric<Vector<VectorCols>, Matrix<Rows, Cols>>(inMatrix, outEigVec, outEigVal);
        return CreateTuple(r, outEigVec, outEigVal);
    }, "matrix"_a, doc);
}

void BindEigenValueSymmetric(nb::module_ &m) {
    auto doc =  "Function to determine the eigen vectors and values of a N x N real symmetric matrix\n"
                "by Jacobi transformations. This method is most suitable for N < 10.\n"
                "Taken and adapted from Numerical Recipies paragraph 11.1\n"
                "An eigen vector is a vector v for which \\f$A \\: v = \\lambda \\: v\\f$\n"
                "Where:\n"
                "A: A square matrix.\n"
                "\\f$\\lambda\\f$: a non-zero constant value.\n"
                "Args:\n"
                "    matrix (Matrix): is the matrix of which to return the eigenvalues and vectors.\n"
                "Returns:\n"
                "    tuple:\n"
                "    - bool: Results\n"
                "    - Matrix: will contain a matrix whose columns contain the normalized eigenvectors (must be identity before call).\n"
                "    - Vector: will contain the eigenvalues.\n"
                "References: \n"
                "    https://en.wikipedia.org/wiki/Eigenvalues_and_eigenvectors\n"
                "Matrix is a matrix type, which has dimensions N x N.";

    BindInst<3, 3, 3>(m, "eigen_value_symmetric_M3X3_V3", doc);

    BindInst<4, 4, 4>(m, "eigen_value_symmetric_M4X4_V4");
}