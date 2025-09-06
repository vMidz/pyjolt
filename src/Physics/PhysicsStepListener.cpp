#include "Common.h"
#include <Jolt/Physics/PhysicsStepListener.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <nanobind/trampoline.h>

class PyPhysicsStepListener : public PhysicsStepListener {
  public:
    NB_TRAMPOLINE(PhysicsStepListener, 1);

    ~PyPhysicsStepListener() override = default;

    void OnStep(const PhysicsStepListenerContext &inContext) override {

        NB_OVERRIDE_PURE_NAME(
            "on_step",
            OnStep,
            inContext);
    }
};

void BindPhysicsStepListener(nb::module_ &m) {
    nb::class_<PhysicsStepListenerContext> physicsStepListenerContextCls(m, "PhysicsStepListenerContext",
        "Context information for the step listener");
    physicsStepListenerContextCls
        .def_rw("delta_time", &PhysicsStepListenerContext::mDeltaTime,
            "Delta time of the current step")
        .def_rw("is_first_step", &PhysicsStepListenerContext::mIsFirstStep,
            "True if this is the first step")
        .def_rw("is_last_step", &PhysicsStepListenerContext::mIsLastStep,
            "True if this is the last step")
        .def_rw("physics_system", &PhysicsStepListenerContext::mPhysicsSystem,
            "The physics system that is being stepped");

    nb::class_<PhysicsStepListener, PyPhysicsStepListener> physicsStepListenerCls(m, "PhysicsStepListener",
            "A listener class that receives a callback before every physics simulation step");
    physicsStepListenerCls
        .def("on_step", &PhysicsStepListener::OnStep, "context"_a,
            "Called before every simulation step (received inCollisionSteps times for every PhysicsSystem::Update(...) call)\n"
            "This is called while all body and constraint mutexes are locked. You can read/write bodies and constraints but not add/remove them.\n"
            "Multiple listeners can be executed in parallel and it is the responsibility of the listener to avoid race conditions.\n"
            "The best way to do this is to have each step listener operate on a subset of the bodies and constraints\n"
            "and making sure that these bodies and constraints are not touched by any other step listener.\n"
            "Note that this function is not called if there aren't any active bodies or when the physics system is updated with 0 delta time.");
}