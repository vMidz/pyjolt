#include "Common.h"
#include <Jolt/Core/LinearCurve.h>
#include <Jolt/Core/StreamIn.h>
#include <Jolt/Core/StreamOut.h>

void BindLinearCurve(nb::module_ &m) {
    nb::class_<LinearCurve> linear_curve_cls(m, "LinearCurve", "A set of points (x, y) that form a linear curve");
    linear_curve_cls
        .def(nb::init<>())
        .def("clear", &LinearCurve::Clear, "Remove all points")
        .def("reserve", &LinearCurve::Reserve, "num_points"_a, "Reserve memory for inNumPoints points")
        .def("add_point", &LinearCurve::AddPoint, "x"_a, "y"_a,
            "Add a point to the curve. Points must be inserted in ascending X or Sort() needs to be called when all points have been added.\n"
            "Args:\n"
            "    x (float): X value\n"
            "    y (float): Y value")
        .def("sort", &LinearCurve::Sort, "Sort the points on X ascending")
        .def("get_min_x", &LinearCurve::GetMinX, "Get the lowest X value")
        .def("get_max_x", &LinearCurve::GetMaxX, "Get the highest X value")
        .def("get_value", &LinearCurve::GetValue, "x"_a, "Sample value on the curve\n"
            "Sample value on the curve.\n"
            "Args:\n"
            "    x (float): X value to sample at\n"
            "Returns:\n"
            "    float: Interpolated Y value.")

        .def("save_binary_state", &LinearCurve::SaveBinaryState, "stream"_a,
            "Saves the state of this object in binary form to inStream.")
        .def("restore_binary_state", &LinearCurve::RestoreBinaryState, "stream"_a,
            "Restore the state of this object from inStream.")

        .def_rw("points", &LinearCurve::mPoints, "The points on the curve, should be sorted ascending by x")

        .def("__getitem__", [](const LinearCurve &curve, size_t i) {
            if (i >= curve.mPoints.size())
                throw nb::index_error();
            return curve.mPoints[i];
        }, "index"_a)

        // Python specific
        .def("__len__", [](const LinearCurve &curve) {
            return curve.mPoints.size();
        })
        .def("__repr__", [](const LinearCurve &curve) {
            std::ostringstream ss;
            ss << "LinearCurve([";
            for (size_t i = 0; i < curve.mPoints.size(); ++i) {
                if (i > 0)
                    ss << ", ";
                ss << "Point(x=" << curve.mPoints[i].mX << ", y=" << curve.mPoints[i].mY << ")";
            }
            ss << "])";
            return nb::str(ss.str().c_str());
        });

    nb::class_<LinearCurve::Point>(linear_curve_cls, "Point", "A point on the curve")
        .def(nb::init<>())
        .def_rw("x", &LinearCurve::Point::mX)
        .def_rw("y", &LinearCurve::Point::mY)
        .def("__repr__", [](const LinearCurve::Point &p) {
            return nb::str("Point({}, {})").format(p.mX, p.mY);
        });
}