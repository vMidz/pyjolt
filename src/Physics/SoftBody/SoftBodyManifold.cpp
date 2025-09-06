#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodyManifold.h>

void BindSoftBodyManifold(nb::module_ &m) {
    nb::class_<SoftBodyManifold> softBodyManifoldCls(m, "SoftBodyManifold",
        "An interface to query which vertices of a soft body are colliding with other bodies");
    softBodyManifoldCls
        .def("get_vertices", &SoftBodyManifold::GetVertices,
            "Get the vertices of the soft body for iterating")
        .def("has_contact", &SoftBodyManifold::HasContact, "vertex"_a,
            "Check if a vertex has collided with something in this update")
        .def("get_local_contact_point", &SoftBodyManifold::GetLocalContactPoint, "vertex"_a,
            "Get the local space contact point (multiply by GetCenterOfMassTransform() of the soft body to get world space)")
        .def("get_contact_normal", &SoftBodyManifold::GetContactNormal, "vertex"_a,
            "Get the contact normal for the vertex (assumes there is a contact).")
        .def("get_contact_body_id", &SoftBodyManifold::GetContactBodyID, "vertex"_a,
            "Get the body with which the vertex has collided in this update")
        .def("get_num_sensor_contacts", &SoftBodyManifold::GetNumSensorContacts,
            "Get the number of sensors that are in contact with the soft body")
        .def("get_sensor_contact_body_id", &SoftBodyManifold::GetSensorContactBodyID, "index"_a,
            "Get the i-th sensor that is in contact with the soft body");
}