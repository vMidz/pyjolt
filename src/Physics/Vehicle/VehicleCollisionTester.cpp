#include "Common.h"
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/Vehicle/VehicleConstraint.h>
#include <Jolt/Physics/Vehicle/VehicleCollisionTester.h>
#include <Jolt/Physics/PhysicsSystem.h>

void BindVehicleCollisionTester(nb::module_ &m) {
    nb::class_<VehicleCollisionTester, RefTarget<VehicleCollisionTester>> vehicleCollisionTesterCls(m, "VehicleCollisionTester",
        nb::intrusive_ptr<VehicleCollisionTester>([](VehicleCollisionTester *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Class that does collision detection between wheels and ground");
    vehicleCollisionTesterCls
        // .def(nb::init<>(),
            // "Constructors")
        // .def(nb::init<ObjectLayer>(), "object_layer"_a)
        .def("get_object_layer", &VehicleCollisionTester::GetObjectLayer,
            "Object layer to use for collision detection, this is used when the filters are not overridden")
        .def("set_object_layer", &VehicleCollisionTester::SetObjectLayer, "object_layer"_a)
        .def("set_broad_phase_layer_filter", &VehicleCollisionTester::SetBroadPhaseLayerFilter, "filter"_a,
            "Access to the broad phase layer filter, when set this overrides the object layer supplied in the constructor")
        .def("get_broad_phase_layer_filter", &VehicleCollisionTester::GetBroadPhaseLayerFilter, nb::rv_policy::reference)
        .def("set_object_layer_filter", &VehicleCollisionTester::SetObjectLayerFilter, "filter"_a,
            "Access to the object layer filter, when set this overrides the object layer supplied in the constructor")
        .def("get_object_layer_filter", &VehicleCollisionTester::GetObjectLayerFilter, nb::rv_policy::reference)
        .def("set_body_filter", &VehicleCollisionTester::SetBodyFilter, "filter"_a,
            "Access to the body filter, when set this overrides the default filter that filters out the vehicle body")
        .def("get_body_filter", &VehicleCollisionTester::GetBodyFilter, nb::rv_policy::reference)
        // .def("collide", &VehicleCollisionTester::Collide, "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a, "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a,
            // "Do a collision test with the world.\n"
            // "Args:\n"
            // "    physics_system (PhysicsSystem): The physics system that should be tested against.\n"
            // "    vehicle_constraint (VehicleConstraint): The vehicle constraint.\n"
            // "    wheel_index (int): Index of the wheel that we're testing collision for.\n"
            // "    origin (Vec3): Origin for the test, corresponds to the world space position for the suspension attachment point.\n"
            // "    direction (Vec3): Direction for the test (unit vector, world space).\n"
            // "    vehicle_body_id (BodyID): This body should be filtered out during collision detection to avoid self collisions.\n"
            // "    body (Body): Body that the wheel collided with.\n"
            // "    sub_shape_id (SubShapeID): Sub shape ID that the wheel collided with.\n"
            // "    contact_position (RVec3): Contact point between wheel and floor, in world space.\n"
            // "    contact_normal (Vec3): Contact normal between wheel and floor, pointing away from the floor.\n"
            // "    suspension_length (float): New length of the suspension [0, inSuspensionMaxLength].\n"
            // "Returns:\n"
            // "    bool: True when collision found, false if not.")
        // .def("predict_contact_properties", &VehicleCollisionTester::PredictContactProperties, "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a, "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a,
            // "Do a cheap contact properties prediction based on the contact properties from the last collision test (provided as input parameters).\n"
            // "Args:\n"
            // "    physics_system (PhysicsSystem): The physics system that should be tested against.\n"
            // "    vehicle_constraint (VehicleConstraint): The vehicle constraint.\n"
            // "    wheel_index (int): Index of the wheel that we're testing collision for.\n"
            // "    origin (Vec3): Origin for the test, corresponds to the world space position for the suspension attachment point.\n"
            // "    direction (Vec3): Direction for the test (unit vector, world space).\n"
            // "    vehicle_body_id (BodyID): The body ID for the vehicle itself.\n"
            // "    body (Body): Body that the wheel previously collided with.\n"
            // "    sub_shape_id (SubShapeID): Sub shape ID that the wheel collided with during the last check.\n"
            // "    contact_position (RVec3): Contact point between wheel and floor during the last check, in world space.\n"
            // "    contact_normal (Vec3): Contact normal between wheel and floor during the last check, pointing away from the floor.\n"
            // "    suspension_length (float): New length of the suspension [0, inSuspensionMaxLength].")
            ;

    nb::class_<VehicleCollisionTesterRay, VehicleCollisionTester> vehicleCollisionTesterRayCls(m, "VehicleCollisionTesterRay",
        "Collision tester that tests collision using a raycast");
    vehicleCollisionTesterRayCls
        .def(nb::init<ObjectLayer, Vec3Arg, float>(), "object_layer"_a, "up"_a, "max_slope_angle"_a = 1.3962634f,    // TODO default values
            "Constructor.\n"
            "Args:\n"
            "    object_layer (ObjectLayer): Object layer to test collision with.\n"
            "    up (Vec3): World space up vector, used to avoid colliding with vertical walls.\n"
            "    max_slope_angle (float): Max angle (rad) that is considered for colliding wheels. This is to avoid colliding with vertical walls.")
        // .def("collide", &VehicleCollisionTesterRay::Collide,
            // "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a,
            // "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a)
        // .def("predict_contact_properties", &VehicleCollisionTesterRay::PredictContactProperties,
        //     "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a,
        //     "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a)
            ;

    nb::class_<VehicleCollisionTesterCastSphere, VehicleCollisionTester> vehicleCollisionTesterCastSphereCls(m, "VehicleCollisionTesterCastSphere",
        "Collision tester that tests collision using a sphere cast");
    vehicleCollisionTesterCastSphereCls
        .def(nb::init<ObjectLayer, float, Vec3Arg, float >(), "object_layer"_a, "radius"_a, "up"_a = Vec3::sAxisY (), "max_slope_angle"_a = DegreesToRadians (80.0f),
            "Constructor.\n"
            "Args:\n"
            "    object_layer (ObjectLayer): Object layer to test collision with.\n"
            "    up (Vec3): World space up vector, used to avoid colliding with vertical walls.\n"
            "    radius (float): Radius of sphere.\n"
            "    max_slope_angle (float): Max angle (rad) that is considered for colliding wheels. This is to avoid colliding with vertical walls.")
        // .def("collide", &VehicleCollisionTesterCastSphere::Collide,
            // "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a,
            // "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a)
        // .def("predict_contact_properties", &VehicleCollisionTesterCastSphere::PredictContactProperties,
        //     "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a,
        //     "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a)
            ;

    nb::class_<VehicleCollisionTesterCastCylinder, VehicleCollisionTester> vehicleCollisionTesterCastCylinderCls(m, "VehicleCollisionTesterCastCylinder",
        "Collision tester that tests collision using a cylinder shape");
    vehicleCollisionTesterCastCylinderCls
        .def(nb::init<ObjectLayer, float>(), "object_layer"_a, "convex_radius_fraction"_a = 0.1f,
            "Constructor.\n"
            "Args:\n"
            "    object_layer (ObjectLayer): Object layer to test collision with.\n"
            "    convex_radius_fraction (float): Fraction of half the wheel width (or wheel radius if it is smaller) that is used as the convex radius.")
        // .def("collide", &VehicleCollisionTesterCastCylinder::Collide,
            // "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a,
            // "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a)
        // .def("predict_contact_properties", &VehicleCollisionTesterCastCylinder::PredictContactProperties,
            // "physics_system"_a, "vehicle_constraint"_a, "wheel_index"_a, "origin"_a, "direction"_a,
            // "vehicle_body_id"_a, "body"_a, "sub_shape_id"_a, "contact_position"_a, "contact_normal"_a, "suspension_length"_a)
            ;
}