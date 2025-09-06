#include "Common.h"
#include <Jolt/Physics/IslandBuilder.h>
#include <Jolt/Core/TempAllocator.h>

void BindIslandBuilder(nb::module_ &m) {
    nb::class_<IslandBuilder, NonCopyable> islandBuilderCls(m, "IslandBuilder",
        "Keeps track of connected bodies and builds islands for multithreaded velocity/position update");
    islandBuilderCls
        .def("init", &IslandBuilder::Init, "max_active_bodies"_a,
            "Initialize the island builder with the maximum amount of bodies that could be active")
        .def("prepare_contact_constraints", &IslandBuilder::PrepareContactConstraints, "max_contact_constraints"_a, "temp_allocator"_a,
            "Prepare for simulation step by allocating space for the contact constraints")
        .def("prepare_non_contact_constraints", &IslandBuilder::PrepareNonContactConstraints, "num_constraints"_a, "temp_allocator"_a,
            "Prepare for simulation step by allocating space for the non-contact constraints")
        .def("link_bodies", &IslandBuilder::LinkBodies, "first"_a, "second"_a,
            "Link two bodies by their index in the BodyManager::mActiveBodies list to form islands")
        .def("link_constraint", &IslandBuilder::LinkConstraint, "constraint_index"_a, "first"_a, "second"_a,
            "Link a constraint to a body by their index in the BodyManager::mActiveBodies")
        .def("link_contact", &IslandBuilder::LinkContact, "contact_index"_a, "first"_a, "second"_a,
            "Link a contact to a body by their index in the BodyManager::mActiveBodies")
        .def("finalize", &IslandBuilder::Finalize, "active_bodies"_a, "num_active_bodies"_a, "num_contacts"_a, "temp_allocator"_a,
            "Finalize the islands after all bodies have been Link()-ed")
        .def("get_num_islands", &IslandBuilder::GetNumIslands,
            "Get the amount of islands formed")
        // .def("get_bodies_in_island", &IslandBuilder::GetBodiesInIsland, "island_index"_a, "bodies_begin"_a, "bodies_end"_a,  // TODO:
            // "Get iterator for a particular island, return false if there are no constraints")
        // .def("get_constraints_in_island", &IslandBuilder::GetConstraintsInIsland, "island_index"_a, "constraints_begin"_a, "constraints_end"_a)
        // .def("get_contacts_in_island", &IslandBuilder::GetContactsInIsland, "island_index"_a, "contacts_begin"_a, "contacts_end"_a)
        .def("set_num_position_steps", &IslandBuilder::SetNumPositionSteps, "island_index"_a, "num_position_steps"_a,
            "The number of position iterations for each island")
        .def("get_num_position_steps", &IslandBuilder::GetNumPositionSteps, "island_index"_a)
        .def("reset_islands", &IslandBuilder::ResetIslands, "temp_allocator"_a,
            "After you're done calling the three functions above, call this function to free associated data");
}