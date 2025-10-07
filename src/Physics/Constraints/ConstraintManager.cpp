#include "Common.h"
#include <Jolt/Physics/Constraints/ConstraintManager.h>
#include <Jolt/Physics/Constraints/CalculateSolverSteps.h>
#include <Jolt/Physics/IslandBuilder.h>
#include <Jolt/Physics/StateRecorder.h>
#include <Jolt/Physics/PhysicsLock.h>
#include <Jolt/Core/Profiler.h>
#include <Jolt/Core/QuickSort.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <Jolt/Physics/Body/BodyManager.h>

void BindConstraintManager(nb::module_ &m) {
     // TODO:
    nb::class_<ConstraintManager, NonCopyable> constraintManagerCls(m, "ConstraintManager",
        "A constraint manager manages all constraints of the same type");
    constraintManagerCls
#ifdef JPH_ENABLE_ASSERTS
        .def(nb::init<PhysicsLockContext>(), "context"_a,
            "Constructor")
#endif
        // .def("add", &ConstraintManager::Add, "constraints"_a, "number"_a,
            // "Add a new constraint. This is thread safe.")
        // .def("remove", &ConstraintManager::Remove, "constraint"_a, "number"_a,
            // "Remove a constraint. This is thread safe.")
        .def("get_constraints", &ConstraintManager::GetConstraints,
            "Get a list of all constraints")
        .def("get_num_constraints", &ConstraintManager::GetNumConstraints,
            "Get total number of constraints")
        // .def("get_active_constraints", &ConstraintManager::GetActiveConstraints, "start_constraint_idx"_a, "end_constraint_idx"_a, "active_constraints"_a, "num_active_constraints"_a,
            // "Determine the active constraints of a subset of the constraints")
        // .def_static("build_islands", &ConstraintManager::sBuildIslands, "active_constraints"_a, "num_active_constraints"_a, "builder"_a, "body_manager"_a,
            // "Link bodies to form islands")
        // .def_static("sort_constraints", &ConstraintManager::sSortConstraints, "active_constraints"_a, "constraint_idx_begin"_a, "constraint_idx_end"_a,
            // "In order to have a deterministic simulation, we need to sort the constraints of an island before solving them")
        // .def_static("setup_velocity_constraints", &ConstraintManager::sSetupVelocityConstraints, "active_constraints"_a, "num_active_constraints"_a, "delta_time"_a,
            // "Prior to solving the velocity constraints, you must call SetupVelocityConstraints once to precalculate values that are independent of velocity")
        // .def_static("warm_start_velocity_constraints", &ConstraintManager::sWarmStartVelocityConstraints, "active_constraints"_a, "constraint_idx_begin"_a, "constraint_idx_end"_a, "warm_start_impulse_ratio"_a, "callback"_a,
            // "Apply last frame's impulses, must be called prior to SolveVelocityConstraints")
        // .def_static("solve_velocity_constraints", &ConstraintManager::sSolveVelocityConstraints, "active_constraints"_a, "constraint_idx_begin"_a, "constraint_idx_end"_a, "delta_time"_a,
            // "This function is called multiple times to iteratively come to a solution that meets all velocity constraints")
        // .def_static("solve_position_constraints", &ConstraintManager::sSolvePositionConstraints, "active_constraints"_a, "constraint_idx_begin"_a, "constraint_idx_end"_a, "delta_time"_a, "baumgarte"_a,
            // "This function is called multiple times to iteratively come to a solution that meets all position constraints")
        .def("draw_constraints", &ConstraintManager::DrawConstraints, "renderer"_a,
            "Draw all constraints")
        .def("draw_constraint_limits", &ConstraintManager::DrawConstraintLimits, "renderer"_a,
            "Draw all constraint limits")
        .def("draw_constraint_reference_frame", &ConstraintManager::DrawConstraintReferenceFrame, "renderer"_a,
            "Draw all constraint reference frames")
        .def("save_state", &ConstraintManager::SaveState, "stream"_a, "filter"_a,
            "Save state of constraints")
        .def("restore_state", &ConstraintManager::RestoreState, "stream"_a,
            "Restore the state of constraints. Returns false if failed.")
        .def("lock_all_constraints", &ConstraintManager::LockAllConstraints,
            "Lock all constraints. This should only be done during PhysicsSystem::Update().")
        .def("unlock_all_constraints", &ConstraintManager::UnlockAllConstraints);
}