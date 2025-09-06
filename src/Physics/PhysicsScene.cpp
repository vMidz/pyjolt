#include "Common.h"
#include <Jolt/Physics/PhysicsScene.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include "BindingUtility/Stream.h"

void BindPhysicsScene(nb::module_ &m) {
    nb::class_<PhysicsScene, RefTarget<PhysicsScene>> physicsSceneCls(m,"PhysicsScene",
        nb::intrusive_ptr<PhysicsScene>([](PhysicsScene *o, PyObject *po) noexcept {
            o->set_self_py(po);
        }), "Contains the creation settings of a set of bodies");
    physicsSceneCls
        .def(nb::init<>())
        .def("add_body", &PhysicsScene::AddBody, "body"_a,
            "Add a body to the scene")
        .def("add_constraint", &PhysicsScene::AddConstraint, "constraint"_a, "body1"_a, "body2"_a,
            "Add a constraint to the scene.\n"
            "Args:\n"
            "    constraint (TwoBodyConstraintSettings*): Constraint settings.\n"
            "    body1 (int): Index in the bodies list of first body to attach constraint to.\n"
            "    body2 (int): Index in the bodies list of the second body to attach constraint to.")
        .def("add_soft_body", &PhysicsScene::AddSoftBody, "soft_body"_a,
            "Add a soft body to the scene")
        .def("get_num_bodies", &PhysicsScene::GetNumBodies,
            "Get number of bodies in this scene")
        .def("get_bodies", nb::overload_cast<>(&PhysicsScene::GetBodies),
            "Access to the body settings for this scene")
        .def("get_num_constraints", &PhysicsScene::GetNumConstraints,
            "Get number of constraints in this scene")
        .def("get_constraints", nb::overload_cast<>(&PhysicsScene::GetConstraints),
            "Access to the constraints for this scene")
        .def("get_num_soft_bodies", &PhysicsScene::GetNumSoftBodies,
            "Get number of bodies in this scene")
        .def("get_soft_bodies", nb::overload_cast<>(&PhysicsScene::GetSoftBodies),
            "Access to the soft body settings for this scene")
        .def("create_bodies", &PhysicsScene::CreateBodies, "system"_a,
            "Instantiate all bodies, returns false if not all bodies could be created")
        .def("fix_invalid_scales", &PhysicsScene::FixInvalidScales,
            "Go through all body creation settings and fix shapes that are scaled incorrectly (note this will change the scene a bit).\n"
            "Returns:\n"
            "    bool: False when not all scales could be fixed.")
        .def("save_binary_state", [](const PhysicsScene &self, nb::object py_stream, bool inSaveShapes, bool inSaveGroupFilter){
            PythonStreamOut stream_adapter(py_stream);
            self.SaveBinaryState(stream_adapter, inSaveShapes, inSaveGroupFilter);
            if (stream_adapter.IsFailed())
                throw std::runtime_error("Failed to save binary state due to a Python I/O error.");
            }, "stream"_a, "save_shapes"_a, "save_group_filter"_a,
            "Saves the state of this object in binary form to inStream.\n"
            "Args:\n"
            "    stream (StreamOut): The stream to save the state to.\n"
            "    save_shapes (bool): If the shapes should be saved as well (these could be shared between physics scenes, in which case the calling application may want to write custom code to restore them).\n"
            "    save_group_filter (bool): If the group filter should be saved as well (these could be shared).")
        .def_static("restore_from_binary_state", [](nb::object py_stream) {
            PythonStreamIn stream_adapter(py_stream);
            return PhysicsScene::sRestoreFromBinaryState(stream_adapter);
        }, "stream"_a, "Restore a saved scene from inStream")
        .def("from_physics_system", &PhysicsScene::FromPhysicsSystem, "system"_a,
            "For debugging purposes: Construct a scene from the current state of the physics system")
        .def_ro_static("C_FIXED_TO_WORLD", &PhysicsScene::cFixedToWorld,
            "Body constant to use to indicate that the constraint is attached to the fixed world");

    nb::class_<PhysicsScene::ConnectedConstraint> connectedConstraintCls(physicsSceneCls, "ConnectedConstraint",
        "A constraint and how it is connected to the bodies in the scene");
    connectedConstraintCls
        .def(nb::init<>())
        .def(nb::init<const TwoBodyConstraintSettings *, uint, uint>(), "settings"_a, "body1"_a, "body2"_a)
        .def_rw("settings", &PhysicsScene::ConnectedConstraint::mSettings,
            "Constraint settings")
        .def_rw("body1", &PhysicsScene::ConnectedConstraint::mBody1,
            "Index of first body (in mBodies)")
        .def_rw("body2", &PhysicsScene::ConnectedConstraint::mBody2,
            "Index of second body (in mBodies)");
}