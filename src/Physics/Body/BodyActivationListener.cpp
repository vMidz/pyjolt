#include "Common.h"
#include <nanobind/trampoline.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyID.h>

class PyBodyActivationListener : public BodyActivationListener {
  public:
    NB_TRAMPOLINE(BodyActivationListener, 2);

    void OnBodyActivated(const BodyID &inBodyID, uint64 inBodyUserData) override {
        NB_OVERRIDE_PURE_NAME(
            "on_body_activated",
            OnBodyActivated,
            inBodyID,
            inBodyUserData);
    }

    void OnBodyDeactivated(const BodyID &inBodyID, uint64 inBodyUserData) override {
        NB_OVERRIDE_PURE_NAME(
            "on_body_deactivated",
            OnBodyDeactivated,
            inBodyID,
            inBodyUserData);
    }
};

void BindBodyActivationListener(nb::module_ &m) {
    nb::class_<BodyActivationListener, PyBodyActivationListener>(m, "BodyActivationListener",
        "A listener class that receives events when a body activates or deactivates.\n"
        "It can be registered with the BodyManager (or PhysicsSystem).")
        .def(nb::init<>())
        .def("OnBodyActivated", &BodyActivationListener::OnBodyActivated,
            "body_id"_a, "body_user_data"_a,
            "Called whenever a body activates, note this can be called from any thread so make sure your code is thread safe.\n"
            "At the time of the callback the body inBodyID will be locked and no bodies can be written/activated/deactivated from the callback.")

        .def("OnBodyDeactivated", &BodyActivationListener::OnBodyDeactivated,
            "body_id"_a, "body_user_data"_a,
            "Called whenever a body deactivates, note this can be called from any thread so make sure your code is thread safe.\n"
            "At the time of the callback the body inBodyID will be locked and no bodies can be written/activated/deactivated from the callback.");
}