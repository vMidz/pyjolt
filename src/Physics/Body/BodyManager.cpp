#include "Common.h"
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Renderer/DebugRenderer.h>

void BindBodyManager(nb::module_ &m) {
    nb::class_<BodyManager, NonCopyable> bodyManagerCls(m, "BodyManager",
        "Class that contains all bodies");
    bodyManagerCls
        .def("init", &BodyManager::Init,
            "max_bodies"_a, "num_body_mutexes"_a, "layer_interface"_a,
            "Initialize the manager")
        .def("get_num_bodies", &BodyManager::GetNumBodies,
            "Gets the current amount of bodies that are in the body manager")
        .def("get_max_bodies", &BodyManager::GetMaxBodies,
            "Gets the max bodies that we can support")
        .def("get_body_stats", &BodyManager::GetBodyStats,
            "Get stats about the bodies in the body manager (slow, iterates through all bodies)")
        .def("allocate_body", &BodyManager::AllocateBody,
            "body_creation_settings"_a, nb::rv_policy::reference,
            "Create a body using creation settings. The returned body will not be part of the body manager yet.")
        .def("allocate_soft_body", &BodyManager::AllocateSoftBody,
            "soft_body_creation_settings"_a, nb::rv_policy::reference,
            "Create a soft body using creation settings. The returned body will not be part of the body manager yet.")
        .def("free_body", &BodyManager::FreeBody,
            "body"_a, "Free a body that has not been added to the body manager yet (if it has, use DestroyBodies).")
        .def("add_body", &BodyManager::AddBody,
            "body"_a, "Add a body to the body manager, assigning it the next available ID. Returns false if no more IDs are available.")
        .def("add_body_with_custom_id", &BodyManager::AddBodyWithCustomID,
            "body"_a, "body_id"_a, "Add a body to the body manager, assigning it a custom ID. Returns false if the ID is not valid.")
        .def("remove_bodies", [](BodyManager &self, const Array<BodyID> &inBodyIDs) {
            if (!inBodyIDs.empty()) {
                BodyVector removed_bodies_temp;
                removed_bodies_temp.resize(inBodyIDs.size());
                self.RemoveBodies(inBodyIDs.data(), (int)inBodyIDs.size(), removed_bodies_temp.data());
            }
        }, "body_ids"_a, "Remove a list of bodies from the body manager")

        .def("destroy_bodies", &BodyManager::DestroyBodies,
            "body_i_ds"_a, "number"_a, "Remove a set of bodies from the body manager and frees them.")
        .def("activate_bodies", &BodyManager::ActivateBodies,
            "body_i_ds"_a, "number"_a, "Activate a list of bodies.\n"
            "This function should only be called when an exclusive lock for the bodies are held.")
        .def("deactivate_bodies", &BodyManager::DeactivateBodies,
            "body_i_ds"_a, "number"_a, "Deactivate a list of bodies.\n"
            "This function should only be called when an exclusive lock for the bodies are held.")
        .def("set_motion_quality", &BodyManager::SetMotionQuality, "body"_a, "motion_quality"_a, "Update the motion quality for a body")
        .def("get_active_bodies", &BodyManager::GetActiveBodies,
            "type"_a, "body_i_ds"_a, "Get copy of the list of active bodies under protection of a lock.")
        .def("get_active_bodies_unsafe", &BodyManager::GetActiveBodiesUnsafe, "type"_a, nb::rv_policy::reference,
            "Get the list of active bodies. Note: Not thread safe. The active bodies list can change at any moment.")
        .def("get_num_active_bodies", &BodyManager::GetNumActiveBodies, "type"_a, "Get the number of active bodies.")
        .def("get_num_active_ccd_bodies", &BodyManager::GetNumActiveCCDBodies,
            "Get the number of active bodies that are using continuous collision detection")
        .def("set_body_activation_listener", &BodyManager::SetBodyActivationListener,
            "listener"_a, "Listener that is notified whenever a body is activated/deactivated")
        .def("get_body_activation_listener", &BodyManager::GetBodyActivationListener, nb::rv_policy::reference)
        .def_static("is_valid_body_pointer", &BodyManager::sIsValidBodyPointer,
            "body"_a, "Check if this is a valid body pointer. When a body is freed the memory that the pointer occupies is reused to store a freelist.")
        .def("get_bodies", nb::overload_cast<>(&BodyManager::GetBodies),
            "Get all bodies. Note that this can contain invalid body pointers, call sIsValidBodyPointer to check.")
        .def("get_body_i_ds", &BodyManager::GetBodyIDs, "bodies"_a, "Get all body IDs under the protection of a lock")
        .def("get_body", nb::overload_cast<const BodyID &>(&BodyManager::GetBody), "id"_a, "Access a body (not protected by lock)")
        .def("try_get_body", nb::overload_cast<const BodyID &>(&BodyManager::TryGetBody),
            "id"_a, nb::rv_policy::reference, "Access a body, will return a nullptr if the body ID is no longer valid (not protected by lock)")
        .def("get_mutex_for_body", &BodyManager::GetMutexForBody, "id"_a, "Access the mutex for a single body")
        .def("get_all_bodies_mutex_mask", &BodyManager::GetAllBodiesMutexMask, "@name Batch body mutex access (do not use directly)\n")
        .def("get_mutex_mask", &BodyManager::GetMutexMask, "bodies"_a, "number"_a)
        .def("lock_read", &BodyManager::LockRead, "mutex_mask"_a)
        .def("unlock_read", &BodyManager::UnlockRead, "mutex_mask"_a)
        .def("lock_write", &BodyManager::LockWrite, "mutex_mask"_a)
        .def("unlock_write", &BodyManager::UnlockWrite, "mutex_mask"_a)
        .def("lock_all_bodies", &BodyManager::LockAllBodies,
            "Lock all bodies. This should only be done during PhysicsSystem::Update().")
        .def("unlock_all_bodies", &BodyManager::UnlockAllBodies,
            "Unlock all bodies. This should only be done during PhysicsSystem::Update().")
        .def("set_body_object_layer_internal", &BodyManager::SetBodyObjectLayerInternal,
            "body"_a, "layer"_a,
            "Function to update body's layer (should only be called by the BodyInterface since it also requires updating the broadphase)")
        .def("invalidate_contact_cache_for_body", &BodyManager::InvalidateContactCacheForBody,
            "body"_a,
            "Set the Body::EFlags::InvalidateContactCache flag for the specified body. This means that the collision cache is invalid for any body pair involving that body until the next physics step.")
        .def("validate_contact_cache_for_all_bodies", &BodyManager::ValidateContactCacheForAllBodies,
            "Reset the Body::EFlags::InvalidateContactCache flag for all bodies. All contact pairs in the contact cache will now by valid again.")
        .def("save_state", &BodyManager::SaveState,
            "stream"_a, "filter"_a, "Saving state for replay")
        .def("restore_state", &BodyManager::RestoreState,
            "stream"_a, "Restoring state for replay. Returns false if failed.")
        .def("save_body_state", &BodyManager::SaveBodyState,
            "body"_a, "stream"_a, "Save the state of a single body for replay")
        .def("restore_body_state", &BodyManager::RestoreBodyState,
            "body"_a, "stream"_a, "Save the state of a single body for replay")
        .def("draw", &BodyManager::Draw, "settings"_a, "physics_settings"_a,
            "renderer"_a, "body_filter"_a = nullptr,
            "Draw the state of the bodies (debugging purposes)")
        .def("set_active_bodies_locked", &BodyManager::SetActiveBodiesLocked,
            "locked"_a, "Lock the active body list, asserts when Activate/DeactivateBody is called.")
        .def("validate_active_body_bounds", &BodyManager::ValidateActiveBodyBounds,
            "Validate if the cached bounding boxes are correct for all active bodies");

    nb::enum_<ESoftBodyConstraintColor>(m, "ESoftBodyConstraintColor", "Defines how to color soft body constraints")
        .value("CONSTRAINT_TYPE", ESoftBodyConstraintColor::ConstraintType,
            "Draw different types of constraints in different colors")
        .value("CONSTRAINT_GROUP", ESoftBodyConstraintColor::ConstraintGroup,
            "Draw constraints in the same group in the same color, non-parallel group will be red")
        .value("CONSTRAINT_ORDER", ESoftBodyConstraintColor::ConstraintOrder,
            "Draw constraints in the same group in the same color, non-parallel group will be red, and order within each group will be indicated with gradient");

    nb::class_<BodyManager::BodyStats> bodyStatsCls(bodyManagerCls, "BodyStats",
        "Helper struct that counts the number of bodies of each type");
    bodyStatsCls
        .def_rw("num_bodies", &BodyManager::BodyStats::mNumBodies,
            "Total number of bodies in the body manager")
        .def_rw("max_bodies", &BodyManager::BodyStats::mMaxBodies,
            "Max allowed number of bodies in the body manager (as configured in Init(...))")
        .def_rw("num_bodies_static", &BodyManager::BodyStats::mNumBodiesStatic,
            "Number of static bodies")
        .def_rw("num_bodies_dynamic", &BodyManager::BodyStats::mNumBodiesDynamic,
            "Number of dynamic bodies")
        .def_rw("num_active_bodies_dynamic", &BodyManager::BodyStats::mNumActiveBodiesDynamic,
            "Number of dynamic bodies that are currently active")
        .def_rw("num_bodies_kinematic", &BodyManager::BodyStats::mNumBodiesKinematic,
            "Number of kinematic bodies")
        .def_rw("num_active_bodies_kinematic", &BodyManager::BodyStats::mNumActiveBodiesKinematic,
            "Number of kinematic bodies that are currently active")
        .def_rw("num_soft_bodies", &BodyManager::BodyStats::mNumSoftBodies,
            "Number of soft bodies")
        .def_rw("num_active_soft_bodies", &BodyManager::BodyStats::mNumActiveSoftBodies,
            "Number of soft bodies that are currently active");

    nb::class_<BodyManager::DrawSettings> drawSettingsCls(bodyManagerCls, "DrawSettings",
        "Draw settings");
    drawSettingsCls
        .def(nb::init<>())
        .def_rw("draw_get_support_function", &BodyManager::DrawSettings::mDrawGetSupportFunction,
            "Draw the GetSupport() function, used for convex collision detection")
        .def_rw("draw_support_direction", &BodyManager::DrawSettings::mDrawSupportDirection,
            "When drawing the support function, also draw which direction mapped to a specific support point")
        .def_rw("draw_get_supporting_face", &BodyManager::DrawSettings::mDrawGetSupportingFace,
            "Draw the faces that were found colliding during collision detection")
        .def_rw("draw_shape", &BodyManager::DrawSettings::mDrawShape,
            "Draw the shapes of all bodies")
        .def_rw("draw_shape_wireframe", &BodyManager::DrawSettings::mDrawShapeWireframe,
            "When mDrawShape is true and this is true, the shapes will be drawn in wireframe instead of solid.")
        .def_rw("draw_shape_color", &BodyManager::DrawSettings::mDrawShapeColor,
            "Coloring scheme to use for shapes")
        .def_rw("draw_bounding_box", &BodyManager::DrawSettings::mDrawBoundingBox,
            "Draw a bounding box per body")
        .def_rw("draw_center_of_mass_transform", &BodyManager::DrawSettings::mDrawCenterOfMassTransform,
            "Draw the center of mass for each body")
        .def_rw("draw_world_transform", &BodyManager::DrawSettings::mDrawWorldTransform,
            "Draw the world transform (which can be different than the center of mass) for each body")
        .def_rw("draw_velocity", &BodyManager::DrawSettings::mDrawVelocity, "Draw the velocity vector for each body")
        .def_rw("draw_mass_and_inertia", &BodyManager::DrawSettings::mDrawMassAndInertia,
            "Draw the mass and inertia (as the box equivalent) for each body")
        .def_rw("draw_sleep_stats", &BodyManager::DrawSettings::mDrawSleepStats,
            "Draw stats regarding the sleeping algorithm of each body")
        .def_rw("draw_soft_body_vertices", &BodyManager::DrawSettings::mDrawSoftBodyVertices, "Draw the vertices of soft bodies")
        .def_rw("draw_soft_body_vertex_velocities", &BodyManager::DrawSettings::mDrawSoftBodyVertexVelocities,
            "Draw the velocities of the vertices of soft bodies")
        .def_rw("draw_soft_body_edge_constraints", &BodyManager::DrawSettings::mDrawSoftBodyEdgeConstraints,
            "Draw the edge constraints of soft bodies")
        .def_rw("draw_soft_body_bend_constraints", &BodyManager::DrawSettings::mDrawSoftBodyBendConstraints,
            "Draw the bend constraints of soft bodies")
        .def_rw("draw_soft_body_volume_constraints", &BodyManager::DrawSettings::mDrawSoftBodyVolumeConstraints,
            "Draw the volume constraints of soft bodies")
        .def_rw("draw_soft_body_skin_constraints", &BodyManager::DrawSettings::mDrawSoftBodySkinConstraints,
            "Draw the skin constraints of soft bodies")
        .def_rw("draw_soft_body_lra_constraints", &BodyManager::DrawSettings::mDrawSoftBodyLRAConstraints,
            "Draw the LRA constraints of soft bodies")
        .def_rw("draw_soft_body_predicted_bounds", &BodyManager::DrawSettings::mDrawSoftBodyPredictedBounds,
            "Draw the predicted bounds of soft bodies")
        .def_rw("draw_soft_body_constraint_color", &BodyManager::DrawSettings::mDrawSoftBodyConstraintColor,
            "Coloring scheme to use for soft body constraints");

    nb::class_<BodyManager::GrantActiveBodiesAccess>(bodyManagerCls, "GrantActiveBodiesAccess",
        "Per thread override of the locked state, to be used by the PhysicsSystem only!");

    nb::enum_<BodyManager::EShapeColor>(bodyManagerCls, "EShapeColor")
        .value("INSTANCE_COLOR", BodyManager::EShapeColor::InstanceColor,
            "Random color per instance")
        .value("SHAPE_TYPE_COLOR", BodyManager::EShapeColor::ShapeTypeColor,
            "Convex = green, scaled = yellow, compound = orange, mesh = red")
        .value("MOTION_TYPE_COLOR", BodyManager::EShapeColor::MotionTypeColor,
            "Static = grey, keyframed = green, dynamic = random color per instance")
        .value("SLEEP_COLOR", BodyManager::EShapeColor::SleepColor,
            "Static = grey, keyframed = green, dynamic = yellow, sleeping = red")
        .value("ISLAND_COLOR", BodyManager::EShapeColor::IslandColor,
            "Static = grey, active = random color per island, sleeping = light grey")
        .value("MATERIAL_COLOR", BodyManager::EShapeColor::MaterialColor,
            "Color as defined by the PhysicsMaterial of the shape");
}