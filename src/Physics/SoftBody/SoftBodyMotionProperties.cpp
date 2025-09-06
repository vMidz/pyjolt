#include "Common.h"
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindSoftBodyMotionProperties(nb::module_ &m) {
    nb::class_<SoftBodyMotionProperties, MotionProperties> softBodyMotionPropertiesCls(m, "SoftBodyMotionProperties",
        "This class contains the runtime information of a soft body.");
    softBodyMotionPropertiesCls
        .def("initialize", &SoftBodyMotionProperties::Initialize, "settings"_a,
            "Initialize the soft body motion properties")
        .def("get_settings", &SoftBodyMotionProperties::GetSettings, nb::rv_policy::reference,
            "Get the shared settings of the soft body")
        .def("get_vertices", nb::overload_cast<>(&SoftBodyMotionProperties::GetVertices),
            "Get the vertices of the soft body")
        .def("get_vertex", nb::overload_cast<uint>(&SoftBodyMotionProperties::GetVertex), "index"_a,
            "Access an individual vertex")
        .def("get_materials", &SoftBodyMotionProperties::GetMaterials,
            "Get the materials of the soft body")
        .def("get_faces", &SoftBodyMotionProperties::GetFaces,
            "Get the faces of the soft body")
        .def("get_face", &SoftBodyMotionProperties::GetFace, "index"_a,
            "Access to an individual face")
        .def("get_num_iterations", &SoftBodyMotionProperties::GetNumIterations,
            "Get the number of solver iterations")
        .def("set_num_iterations", &SoftBodyMotionProperties::SetNumIterations, "num_iterations"_a)
        .def("get_pressure", &SoftBodyMotionProperties::GetPressure,
            "Get the pressure of the soft body")
        .def("set_pressure", &SoftBodyMotionProperties::SetPressure, "pressure"_a)
        .def("get_update_position", &SoftBodyMotionProperties::GetUpdatePosition,
            "Update the position of the body while simulating (set to false for something that is attached to the static world)")
        .def("set_update_position", &SoftBodyMotionProperties::SetUpdatePosition, "update_position"_a)
        .def("get_enable_skin_constraints", &SoftBodyMotionProperties::GetEnableSkinConstraints,
            "Global setting to turn on/off skin constraints")
        .def("set_enable_skin_constraints", &SoftBodyMotionProperties::SetEnableSkinConstraints, "enable_skin_constraints"_a)
        .def("get_skinned_max_distance_multiplier", &SoftBodyMotionProperties::GetSkinnedMaxDistanceMultiplier,
            "Multiplier applied to Skinned::mMaxDistance to allow tightening or loosening of the skin constraints. 0 to hard skin all vertices.")
        .def("set_skinned_max_distance_multiplier", &SoftBodyMotionProperties::SetSkinnedMaxDistanceMultiplier, "skinned_max_distance_multiplier"_a)
        .def("get_local_bounds", &SoftBodyMotionProperties::GetLocalBounds,
            "Get local bounding box")
        .def("get_volume", &SoftBodyMotionProperties::GetVolume,
            "Get the volume of the soft body. Note can become negative if the shape is inside out!")
        .def("calculate_mass_and_inertia", &SoftBodyMotionProperties::CalculateMassAndInertia,
            "Calculate the total mass and inertia of this body based on the current state of the vertices")
        .def("draw_vertices", &SoftBodyMotionProperties::DrawVertices, "renderer"_a, "center_of_mass_transform"_a,
            "Draw the state of a soft body")
        .def("draw_vertex_velocities", &SoftBodyMotionProperties::DrawVertexVelocities, "renderer"_a, "center_of_mass_transform"_a)
        .def("draw_edge_constraints", &SoftBodyMotionProperties::DrawEdgeConstraints, "renderer"_a, "center_of_mass_transform"_a, "constraint_color"_a)
        .def("draw_bend_constraints", &SoftBodyMotionProperties::DrawBendConstraints, "renderer"_a, "center_of_mass_transform"_a, "constraint_color"_a)
        .def("draw_volume_constraints", &SoftBodyMotionProperties::DrawVolumeConstraints, "renderer"_a, "center_of_mass_transform"_a, "constraint_color"_a)
        .def("draw_skin_constraints", &SoftBodyMotionProperties::DrawSkinConstraints, "renderer"_a, "center_of_mass_transform"_a, "constraint_color"_a)
        .def("draw_lra_constraints", &SoftBodyMotionProperties::DrawLRAConstraints, "renderer"_a, "center_of_mass_transform"_a, "constraint_color"_a)
        .def("draw_predicted_bounds", &SoftBodyMotionProperties::DrawPredictedBounds, "renderer"_a, "center_of_mass_transform"_a)
        .def("save_state", &SoftBodyMotionProperties::SaveState, "stream"_a,
            "Saving state for replay")
        .def("restore_state", &SoftBodyMotionProperties::RestoreState, "stream"_a,
            "Restoring state for replay")
        .def("skin_vertices", &SoftBodyMotionProperties::SkinVertices, "center_of_mass_transform"_a, "joint_matrices"_a, "num_joints"_a, "hard_skin_all"_a, "temp_allocator"_a,
            "Skin vertices to supplied joints, information is used by the skinned constraints.\n"
            "Args:\n"
            "    center_of_mass_transform (Mat44): Value of Body::GetCenterOfMassTransform().\n"
            "    joint_matrices (Mat44*): The joint matrices must be expressed relative to inCenterOfMassTransform.\n"
            "    num_joints (int): Indicates how large the inJointMatrices array is (used only for validating out of bounds).\n"
            "    hard_skin_all (bool): Can be used to position all vertices on the skinned vertices and can be used to hard reset the soft body.\n"
            "    temp_allocator (TempAllocator): Allocator.")
        .def("custom_update", &SoftBodyMotionProperties::CustomUpdate, "delta_time"_a, "soft_body"_a, "system"_a,
            "This function allows you to update the soft body immediately without going through the PhysicsSystem.\n"
            "This is useful if the soft body is teleported and needs to 'settle' or it can be used if a the soft body\n"
            "is not added to the PhysicsSystem and needs to be updated manually. One reason for not adding it to the\n"
            "PhyicsSystem is that you might want to update a soft body immediately after updating an animated object\n"
            "that has the soft body attached to it. If the soft body is added to the PhysicsSystem it will be updated\n"
            "by it, so calling this function will effectively update it twice. Note that when you use this function,\n"
            "only the current thread will be used, whereas if you update through the PhysicsSystem, multiple threads may\n"
            "be used.\n"
            "Note that this will bypass any sleep checks. Since the dynamic objects that the soft body touches\n"
            "will not move during this call, there can be simulation artifacts if you call this function multiple times\n"
            "without running the physics simulation step.")
        .def("initialize_update_context", &SoftBodyMotionProperties::InitializeUpdateContext, "delta_time"_a, "soft_body"_a, "system"_a, "context"_a,
            "Initialize the update context. Not part of the public API.")
        .def("determine_colliding_shapes", &SoftBodyMotionProperties::DetermineCollidingShapes, "context"_a, "system"_a, "body_lock_interface"_a,
            "Do a broad phase check and collect all bodies that can possibly collide with this soft body. Not part of the public API.")
        .def("parallel_update", &SoftBodyMotionProperties::ParallelUpdate, "context"_a, "physics_settings"_a,
            "Update the soft body, will process a batch of work. Not part of the public API.")
        .def("update_rigid_body_velocities", &SoftBodyMotionProperties::UpdateRigidBodyVelocities, "context"_a, "body_interface"_a,
            "Update the velocities of all rigid bodies that we collided with. Not part of the public API.");

    nb::enum_<SoftBodyMotionProperties::EStatus>(softBodyMotionPropertiesCls, "EStatus",
            "Return code for ParallelUpdate")
        .value("NO_WORK", SoftBodyMotionProperties::EStatus::NoWork,
            "No work was done because other threads were still working on a batch that cannot run concurrently")
        .value("DID_WORK", SoftBodyMotionProperties::EStatus::DidWork,
            "Work was done to progress the update")
        .value("DONE", SoftBodyMotionProperties::EStatus::Done,
            "All work is done");
}