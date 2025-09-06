#include "Common.h"
#include <Jolt/Physics/Collision/CollideSoftBodyVertexIterator.h>
#include <nanobind/operators.h>

void BindCollideSoftBodyVertexIterator(nb::module_ &m) {
    nb::class_<CollideSoftBodyVertexIterator> collideSoftBodyVertexIteratorCls(m, "CollideSoftBodyVertexIterator",
        "Class that allows iterating over the vertices of a soft body.\n"
        "It tracks the largest penetration and allows storing the resulting collision in a different structure than the soft body vertex itself.");
    collideSoftBodyVertexIteratorCls
        .def(nb::init<>(),
            "Default constructor")
        .def(nb::init<const CollideSoftBodyVertexIterator &>())
        .def(nb::init<const StridedPtr<const Vec3> &, const StridedPtr<const float> &, const StridedPtr<Plane> &,
                const StridedPtr<float> &, const StridedPtr<int> &>(),
                "position"_a, "inv_mass"_a, "collision_plane"_a, "largest_penetration"_a, "colliding_shape_index"_a,
            "Construct using (strided) pointers")
        .def(nb::init<SoftBodyVertex *>(), "vertices"_a,
            "Construct using a soft body vertex")

        .def("assign", &CollideSoftBodyVertexIterator::operator=,
            "Default assignment")
        .def(nb::self != nb::self, "rhs"_a,
            "Equality operator.\n"
            "Note: Only used to determine end iterator, so we only compare position.")
        .def("inc_1", &CollideSoftBodyVertexIterator::operator++,
            "Next vertex")
        .def(nb::self + int(), "offset"_a,
            "Add an offset\n"
            "Note: Only used to determine end iterator, so we only set position.")
        .def("get_position", &CollideSoftBodyVertexIterator::GetPosition,
            "Get the position of the current vertex")
        .def("get_inv_mass", &CollideSoftBodyVertexIterator::GetInvMass,
            "Get the inverse mass of the current vertex")
        .def("update_penetration", &CollideSoftBodyVertexIterator::UpdatePenetration, "largest_penetration"_a,
            "Update penetration of the current vertex\n"
            "Returns:\n"
            "    Returns true if the vertex has the largest penetration so far, this means you need to follow up by calling SetCollision")
        .def("set_collision", &CollideSoftBodyVertexIterator::SetCollision, "collision_plane"_a, "colliding_shape_index"_a,
            "Update the collision of the current vertex");
}