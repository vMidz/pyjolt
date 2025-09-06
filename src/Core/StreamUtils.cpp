#include "Common.h"
#include <Jolt/Core/StreamUtils.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/GroupFilter.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>

void BindStreamUtils(nb::module_ &m) {
    m.def("restore_object", &StreamUtils::RestoreObject<GroupFilter>,
          "stream"_a,
          "restore_binary_state_function"_a,
          "Restore a single object by reading the hash of the type, constructing it and then calling the restore function");

    m.def("save_object_reference", &StreamUtils::SaveObjectReference<GroupFilter>,
          "stream"_a, "object"_a, "object_to_id_map"_a,
          "Save an object reference to a stream. Uses a map to map objects to IDs which is also used to prevent writing duplicates.");

    m.def("restore_object_reference", &StreamUtils::RestoreObjectReference<GroupFilter>,
          "stream"_a, "object_to_id_map"_a,
          "Restore an object reference from stream.");

    m.def("save_object_array", [](StreamOut &inStream, const Array<Ref<PhysicsMaterial>> &inArray, SoftBodySharedSettings::MaterialToIDMap *ioObjectToIDMap) {
        StreamUtils::SaveObjectArray(inStream, inArray, ioObjectToIDMap);
    }, "stream"_a, "array"_a, "object_to_id_map"_a, "Restore an object reference from stream.");

    // Not needed?, can be accessed through PhysicsMaterial::sRestoreFromBinaryState
    m.def("restore_object", [](StreamIn &inStream, int id_to_object_map) -> Result<Ref<PhysicsMaterial>> {
        return StreamUtils::RestoreObject<PhysicsMaterial>(inStream, nullptr);
    }, "stream"_a, "id_to_object_map"_a, "Restore an array of objects from a stream.");

    // m.def("save_object_array", _static_cast<void (*)(StreamOut &,const Array<Ref<PhysicsMaterial>> &,SoftBodySharedSettings::MaterialToIDMap *)>(&StreamUtils::SaveObjectArray),
}
