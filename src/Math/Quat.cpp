#include "Common.h"
#include <random>
#include <Jolt/Math/Quat.h>
#include "BindingUtility/Tuple.h"
#include <nanobind/operators.h>

void BindQuat(nb::module_ &m) {
    nb::class_<Quat>(m, "Quat", "Quaternion class, quaternions are 4 dimensional vectors which can describe rotations in 3 dimensional\n"
                                "space if their length is 1.\n"
                                "They are written as:\n"
                                "\\f$q = w + x : i + y : j + z : k\\f$\n"
                                "or in vector notation:\n"
                                "\\f$q = [w, v] = [w, x, y, z]\\f$\n"
                                "Where:\n"
                                "w = the real part\n"
                                "v = the imaginary part, (x, y, z)\n"
                                "Note that we store the quaternion in a Vec4 as [x, y, z, w] because that makes\n"
                                "it easy to extract the rotation axis of the quaternion:\n"
                                "q = [cos(angle / 2), sin(angle / 2) * rotation_axis]")
        .def(nb::init<>(), "Intentionally not initialized for performance reasons")
        .def(nb::init<const Quat &>(), "quat"_a)
        .def(nb::init<float, float, float, float>(), "x"_a, "y"_a, "z"_a, "w"_a, "Constructor from x, y, z, w components")
        .def(nb::init<Vec4>(), "vector"_a)

        .def("is_close", &Quat::IsClose,
            "other"_a, "max_dist_sq"_a = 1.0e-12f,
            "If this quaternion is close to inRHS. Note that q and -q represent the same rotation, this is not checked here.")
        .def("is_normalized", &Quat::IsNormalized, "tolerance"_a = 1.0e-5f, "Check if the length is 1 +/- tolerance")
        .def("is_nan", &Quat::IsNaN, "If any component of this quaternion is a NaN (not a number)")

        // Component getters and setters
        .def_prop_rw("x", &Quat::GetX, &Quat::SetX, "x"_a, "X component (imaginary part i)")
        .def_prop_rw("y", &Quat::GetY, &Quat::SetY, "y"_a, "Y component (imaginary part j)")
        .def_prop_rw("z", &Quat::GetZ, &Quat::SetZ, "z"_a, "Z component (imaginary part k)")
        .def_prop_rw("w", &Quat::GetW, &Quat::SetW, "w"_a, "W component (real part)")
        .def("set", &Quat::Set, "x"_a, "y"_a, "z"_a, "w"_a, "Set all components")
        .def("get_xyz", &Quat::GetXYZ, "Get the imaginary part of the quaternion as a Vec3")
        .def("get_xyzw", &Quat::GetXYZW, "Get the quaternion as a Vec4")

        .def_static("zero", &Quat::sZero,
            "Returns:\n"
            "    Quat: [0, 0, 0, 0].")
        .def_static("identity", &Quat::sIdentity,
            "Returns:\n"
            "    Quat: [1, 0, 0, 0] (or in storage format Quat(0, 0, 0, 1)).")

        .def_static("rotation", &Quat::sRotation, "axis"_a, "angle"_a, "Create rotation from axis and angle")
        .def("get_axis_angle", [](const Quat &q) {
            Vec3 axis;
            float angle;
            q.GetAxisAngle(axis, angle);
            return CreateTuple(axis, angle);
        }, "Get the axis and angle that represents this quaternion\n"
           "Returns:\n"
           "    tuple:\n"
           "    - Vec3: Axis\n"
           "    - float: Angle")
        .def_static("from_to", &Quat::sFromTo,
            "from_numpy"_a, "to_numpy"_a,
            "Create quaternion that rotates a vector from the direction of inFrom to the direction of inTo along the shortest path.\n"
            "References: \n"
            "    https://www.euclideanspace.com/maths/algebra/vectors/angleBetween/index.htm")
        .def_static("random", []() {
            static std::default_random_engine rd;
            return Quat::sRandom(rd);
        }, "Random unit quaternion, uses std::default_random_engine")
        .def_static("euler_angles", &Quat::sEulerAngles, "angles"_a, "Create quaternion from Euler angles (XYZ order, radians)")

        .def("get_euler_angles", &Quat::GetEulerAngles,
            "Conversion to Euler angles. Rotation order is X then Y then Z (RotZ * RotY * RotX). Angles in radians.")

        .def("length_sq", &Quat::LengthSq,
            "Squared length of quaternion.\n"
            "Returns:\n"
            "    float: Squared length of quaternion (\\f$|v|^2\\f$).")
        .def("length", &Quat::Length,
            "Length of quaternion.\n"
            "Returns:\n"
            "    float: Length of quaternion (\\f$|v|\\f$).")

        .def("normalized", &Quat::Normalized, "Normalize the quaternion (make it length 1)")

        .def("inverse_rotate", &Quat::InverseRotate, "vector"_a, "Rotate a vector by the inverse of this quaternion")
        .def("rotate_axis_x", &Quat::RotateAxisX, "Rotate the vector (1, 0, 0) with this quaternion")
        .def("rotate_axis_y", &Quat::RotateAxisY, "Rotate the vector (0, 1, 0) with this quaternion")
        .def("rotate_axis_z", &Quat::RotateAxisZ, "Rotate the vector (0, 0, 1) with this quaternion")

        .def("dot", &Quat::Dot, "other"_a, "Dot product with another quaternion")
        .def("conjugated", &Quat::Conjugated, "The conjugate [w, -x, -y, -z] is the same as the inverse for unit quaternions")
        .def("inversed", &Quat::Inversed, "Get inverse quaternion")
        .def("ensure_w_positive", &Quat::EnsureWPositive,
            "Ensures that the W component is positive by negating the entire quaternion if it is not. This is useful when you want to store a quaternion as a 3 vector by discarding W and reconstructing it as sqrt(1 - x^2 - y^2 - z^2).")
        .def("get_perpendicular", &Quat::GetPerpendicular, "Get a quaternion that is perpendicular to this quaternion")
        .def("get_rotation_angle", &Quat::GetRotationAngle,
            "axis"_a,
            "Get rotation angle around inAxis (uses Swing Twist Decomposition to get the twist quaternion and uses q(axis, angle) = [cos(angle / 2), axis * sin(angle / 2)])")
        .def("get_twist", &Quat::GetTwist,
            "axis"_a,
            "Swing Twist Decomposition: any quaternion can be split up as:"
            "\\f[q = q_{swing} \\: q_{twist}\\f]\n"
            "where \\f$q_{twist}\\f$ rotates only around axis v.\n"
            "\\f$q_{twist}\\f$ is:\n"
            "\\f[q_{twist} = \\frac{[q_w, q_{ijk} \\cdot v \\: v]}{\\left|[q_w, q_{ijk} \\cdot v \\: v]\\right|}\\f]\n"
            "where q_w is the real part of the quaternion and q_i the imaginary part (a 3 vector).\n"
            "The swing can then be calculated as:\n"
            "\\f[q_{swing} = q \\: q_{twist}^* \\f]\n"
            "Where \\f$q_{twist}^*\\f$ = complex conjugate of \\f$q_{twist}\\f$")

        .def("get_swing_twist", [](const Quat &self) {
            Quat swing, twist;
            self.GetSwingTwist(swing, twist);
            return CreateTuple(swing, twist);
        },  "Decomposes quaternion into swing and twist component:\n"
            "\\f$q = q_{swing} \\: q_{twist}\\f$\n"
            "where \\f$q_{swing} \\: \\hat{x} = q_{twist} \\: \\hat{y} = q_{twist} \\: \\hat{z} = 0\\f$\n"
            "In other words:\n"
            "- \\f$q_{twist}\\f$ only rotates around the X-axis.\n"
            "- \\f$q_{swing}\\f$ only rotates around the Y and Z-axis.\n"
            "References: \n"
            "    Gino van den Bergen - Rotational Joint Limits in Quaternion Space - GDC 2016\n"
            "Returns:\n"
            "    tuple:\n"
            "    - Quat: Swing\n"
            "    - Quat: Twist")

        .def("lerp", &Quat::LERP,
            "destination"_a, "fraction"_a,
            "Linear interpolation between two quaternions (for small steps).\n"
            "Args:\n"
            "    fraction (float): is in the range [0, 1]\n"
            "    destination (Quat): The destination quaternion\n"
            "Returns:\n"
            "    Quat: (1 - inFraction) * this + fraction * inDestination.")

        .def("slerp", &Quat::SLERP,
            "destination"_a, "fraction"_a,
            "Spherical linear interpolation between two quaternions.\n"
            "Args:\n"
            "    fraction (float): is in the range [0, 1]\n"
            "    destination (Quat): The destination quaternion\n"
            "Returns:\n"
            "    Quat: When fraction is zero this quaternion is returned, when fraction is 1 inDestination is returned.\n"
            "When fraction is between 0 and 1 an interpolation along the shortest path is returned.")

        .def_static("load_float3_unsafe", &Quat::sLoadFloat3Unsafe,
            "v"_a, "Load 3 floats from memory (X, Y and Z component and then calculates W) reads 32 bits extra which it doesn't use")

        .def("store_float3", [](const Quat &self) {
            Float3 float_3;
            self.StoreFloat3(&float_3);
            return float_3;
        }, "Store 3 as floats to memory (X, Y and Z component)")

        // Operators
        .def(nb::self == nb::self, "rhs"_a)
        .def(nb::self != nb::self, "rhs"_a)

        .def(nb::self + nb::self, "rhs"_a)
        .def(nb::self += nb::self, "rhs"_a)
        .def(nb::self - nb::self, "rhs"_a)
        .def(nb::self -= nb::self, "rhs"_a)

        .def(nb::self / float(), "rhs"_a)
        .def(nb::self /= float(), "rhs"_a)

        .def(nb::self *= float(), "rhs"_a)
        .def(nb::self * nb::self, "rhs"_a)
        .def(nb::self * float(), "rhs"_a)
        .def(nb::self * Vec3Arg(), "rhs"_a, "Rotate a vector by this quaternion")
        .def(float() * nb::self, "rhs"_a)

        .def(-nb::self)

        // Python string representation
        .def("__repr__", [](const Quat &self) {
            return nb::str("Quat({}, {}, {}, {})").format(self.GetX(), self.GetY(), self.GetZ(), self.GetW());
        });
}