#include "Common.h"
#include <Jolt/Physics/Body/BodyID.h>
#include <nanobind/operators.h>

void BindBodyID(nb::module_ &m) {
    nb::class_<BodyID>(m, "BodyID",
        "ID of a body. This is a way of reasoning about bodies in a multithreaded simulation while avoiding race conditions.")
        .def_ro_static("INVALID_BODY_ID", &BodyID::cInvalidBodyID, "The value for an invalid body ID")
        .def_ro_static("BROAD_PHASE_BIT", &BodyID::cBroadPhaseBit, "This bit is used by the broadphase")
        .def_ro_static("MAX_BODY_INDEX", &BodyID::cMaxBodyIndex, "Maximum value for body index (also the maximum amount of bodies supported - 1)")
        .def_ro_static("MAX_SEQUENCE_NUMBER", &BodyID::cMaxSequenceNumber, "Maximum value for the sequence number")

        .def(nb::init<>(), "Construct invalid body ID")
        .def(nb::init<uint32>(), "id"_a, "Construct from index and sequence number combined in a single uint32 (use with care!)")
        .def(nb::init<uint32, uint8>(), "index"_a, "sequence_number"_a, "Construct from index and sequence number")

        .def("get_index", &BodyID::GetIndex, "Get index in body array")
        .def("get_sequence_number", &BodyID::GetSequenceNumber, "Get sequence number of body.\n"
                                                                "The sequence number can be used to check if a body ID with the same body index has been reused by another body.\n"
                                                                "It is mainly used in multi threaded situations where a body is removed and its body index is immediately reused by a body created from another thread.\n"
                                                                "Functions querying the broadphase can (after acquiring a body lock) detect that the body has been removed (we assume that this won't happen more than 128 times in a row).\n")
        .def("get_index_and_sequence_number", &BodyID::GetIndexAndSequenceNumber, "Returns the index and sequence number combined in an uint32")
        .def("is_invalid", &BodyID::IsInvalid, "Check if the ID is valid")

        .def(nb::self == nb::self, "rhs"_a)
        .def(nb::self != nb::self, "rhs"_a)
        .def(nb::self < nb::self, "rhs"_a)
        .def(nb::self > nb::self, "rhs"_a)

        // Python-specific
        .def("__repr__", [](const BodyID &id) {
            if (id.IsInvalid()) {
                return nb::str("BodyID(invalid)");
            } else {
                return nb::str("BodyID(index={}, seq={})").format(id.GetIndex(), id.GetSequenceNumber());
            }
        })
        .def("__hash__", [](const BodyID &id) {
            return std::hash<uint32_t>{}(id.GetIndexAndSequenceNumber());
        });
}