#include "Common.h"
#include <Jolt/Physics/LargeIslandSplitter.h>
#include <Jolt/Physics/IslandBuilder.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/Constraints/ContactConstraintManager.h>
#include <Jolt/Physics/Constraints/CalculateSolverSteps.h>
#include <Jolt/Physics/Constraints/Constraint.h>
#include <Jolt/Core/TempAllocator.h>

void BindLargeIslandSplitter(nb::module_ &m) {
    nb::class_<LargeIslandSplitter, NonCopyable> largeIslandSplitterCls(m, "LargeIslandSplitter",
        "Assigns bodies in large islands to multiple groups that can run in parallel\n"
        "This basically implements what is described in: High-Performance Physical Simulations on Next-Generation Architecture with Many Cores by Chen et al.\n"
        "See: http://web.eecs.umich.edu/~msmelyan/papers/physsim_onmanycore_itj.pdf section \"PARALLELIZATION METHODOLOGY\"");
    largeIslandSplitterCls
        .def("prepare", &LargeIslandSplitter::Prepare, "island_builder"_a, "num_active_bodies"_a, "temp_allocator"_a,
            "Prepare the island splitter by allocating memory")
        .def("assign_split", &LargeIslandSplitter::AssignSplit, "body1"_a, "body2"_a,
            "Assign two bodies to a split. Returns the split index.")
        .def("assign_to_non_parallel_split", &LargeIslandSplitter::AssignToNonParallelSplit, "body"_a,
            "Force a body to be in a non parallel split. Returns the split index.")
        // .def("split_island", &LargeIslandSplitter::SplitIsland, "island_index"_a, "island_builder"_a, "body_manager"_a, "contact_manager"_a, "active_constraints"_a, "steps_calculator"_a,
            // "Splits up an island, the created splits will be added to the list of batches and can be fetched with FetchNextBatch. Returns false if the island did not need splitting.")
        // .def("fetch_next_batch", [](){      // TODO:

        // }
        // , "split_island_index"_a, "constraints_begin"_a, "constraints_end"_a, "contacts_begin"_a, "contacts_end"_a, "first_iteration"_a,
            // "Fetch the next batch to process, returns a handle in outSplitIslandIndex that must be provided to MarkBatchProcessed when complete")
        // .def("mark_batch_processed", [](LargeIslandSplitter &self) { // TODO:
            // bool lastIteration, finalBatch;
            // self.MarkBatchProcessed(inSplitIslandIndex, inConstraintsBegin, inConstraintsEnd, inContactsBegin, inContactsEnd, lastIteration, finalBatch);
            // return nb::make_tuple(lastIteration, finalBatch);
        // }, "split_island_index"_a, "constraints_begin"_a, "constraints_end"_a, "contacts_begin"_a, "contacts_end"_a, "last_iteration"_a, "final_batch"_a,
        //     "Mark a batch as processed")
        .def("get_island_index", &LargeIslandSplitter::GetIslandIndex, "split_island_index"_a,
            "Get the island index of the island that was split for a particular split island index")
        .def("prepare_for_solve_positions", &LargeIslandSplitter::PrepareForSolvePositions,
            "Prepare the island splitter for iterating over the split islands again for position solving. Marks all batches as startable.")
        .def("reset", &LargeIslandSplitter::Reset, "temp_allocator"_a,
            "Reset the island splitter")
        .def_ro_static("C_NUM_SPLITS", &LargeIslandSplitter::cNumSplits)
        .def_ro_static("C_NON_PARALLEL_SPLIT_IDX", &LargeIslandSplitter::cNonParallelSplitIdx)
        .def_ro_static("C_LARGE_ISLAND_TRESHOLD", &LargeIslandSplitter::cLargeIslandTreshold,
            "If the number of constraints + contacts in an island is larger than this, we will try to split the island");

    nb::class_<LargeIslandSplitter::Split> splitCls(largeIslandSplitterCls, "Split",
        "Describes a split of constraints and contacts");
    splitCls
        .def("get_num_contacts", &LargeIslandSplitter::Split::GetNumContacts)
        .def("get_num_constraints", &LargeIslandSplitter::Split::GetNumConstraints)
        .def("get_num_items", &LargeIslandSplitter::Split::GetNumItems)
        .def_rw("contact_buffer_begin", &LargeIslandSplitter::Split::mContactBufferBegin,
            "Begin of the contact buffer (offset relative to mContactAndConstraintIndices)")
        .def_rw("contact_buffer_end", &LargeIslandSplitter::Split::mContactBufferEnd,
            "End of the contact buffer")
        .def_rw("constraint_buffer_begin", &LargeIslandSplitter::Split::mConstraintBufferBegin,
            "Begin of the constraint buffer (offset relative to mContactAndConstraintIndices)")
        .def_rw("constraint_buffer_end", &LargeIslandSplitter::Split::mConstraintBufferEnd,
            "End of the constraint buffer");

    nb::class_<LargeIslandSplitter::Splits> splitsCls(largeIslandSplitterCls, "Splits",
        "Structure that describes the resulting splits from the large island splitter");
    splitsCls
        .def("get_num_splits", &LargeIslandSplitter::Splits::GetNumSplits)
        .def("get_constraints_in_split", [](const LargeIslandSplitter::Splits &self, uint inSplitIndex) {
            uint32 begin, end;
            self.GetConstraintsInSplit(inSplitIndex, begin, end);
            return nb::make_tuple(begin, end);
        }, "split_index"_a,
           "Output: [begin, end]")
        .def("get_contacts_in_split", [](const LargeIslandSplitter::Splits &self, uint inSplitIndex) {
            uint32 begin, end;
            self.GetContactsInSplit(inSplitIndex, begin, end);
            return nb::make_tuple(begin, end);
        }, "split_index"_a,
           "Output: [begin, end]")
        .def("reset_status", &LargeIslandSplitter::Splits::ResetStatus,
            "Reset current status so that no work can be picked up from this split")
        .def("start_first_batch", &LargeIslandSplitter::Splits::StartFirstBatch,
            "Make the first batch available to other threads")
        .def("fetch_next_batch", [](LargeIslandSplitter::Splits &self) {
            uint32 constraintsBegin, constraintsEnd, contactsBegin, contactsEnd;
            bool firstIteration;
            LargeIslandSplitter::EStatus status = self.FetchNextBatch(constraintsBegin, constraintsEnd, contactsBegin, contactsEnd, firstIteration);
            return nb::make_tuple(status, constraintsBegin, constraintsEnd, contactsBegin, contactsEnd, firstIteration);
        }, "Fetch the next batch to process\n"
            "Output: [status, constraintsBegin, constraintsEnd, contactsBegin, contactsEnd, firstIteration]")
        .def("mark_batch_processed",  [](LargeIslandSplitter::Splits &self, uint inNumProcessed) {
            bool lastIteration, finalBatch;
            self.MarkBatchProcessed(inNumProcessed, lastIteration, finalBatch);
            return nb::make_tuple(lastIteration, finalBatch);
        }, "num_processed"_a,
            "Mark a batch as processed\n"
            "Output: [lastIteration, finalBatch]")
        .def_static("get_iteration", &LargeIslandSplitter::Splits::sGetIteration, "status"_a)
        .def_static("get_split", &LargeIslandSplitter::Splits::sGetSplit, "status"_a)
        .def_static("get_item", &LargeIslandSplitter::Splits::sGetItem, "status"_a)
        // .def_rw("splits", &LargeIslandSplitter::Splits::mSplits,
        //     "Data per split")
        .def_rw("island_index", &LargeIslandSplitter::Splits::mIslandIndex,
            "Index of the island that was split")
        .def_rw("num_splits", &LargeIslandSplitter::Splits::mNumSplits,
            "Number of splits that were created (excluding the non-parallel split)")
        .def_rw("num_iterations", &LargeIslandSplitter::Splits::mNumIterations,
            "Number of iterations to do")
        .def_rw("num_velocity_steps", &LargeIslandSplitter::Splits::mNumVelocitySteps,
            "Number of velocity steps to do (cached for 2nd sub step)")
        .def_rw("num_position_steps", &LargeIslandSplitter::Splits::mNumPositionSteps,
            "Number of position steps to do")
        // .def_rw("status", &LargeIslandSplitter::Splits::mStatus,
        //     "Status of the split, see EIterationStatus")
        // .def_rw("items_processed", &LargeIslandSplitter::Splits::mItemsProcessed,
        //     "Number of items that have been marked as processed")
            ;

    nb::enum_<LargeIslandSplitter::Splits::EIterationStatus>(splitCls, "EIterationStatus")
        .value("STATUS_ITERATION_MASK", LargeIslandSplitter::Splits::EIterationStatus::StatusIterationMask)
        .value("STATUS_ITERATION_SHIFT", LargeIslandSplitter::Splits::EIterationStatus::StatusIterationShift)
        .value("STATUS_SPLIT_MASK", LargeIslandSplitter::Splits::EIterationStatus::StatusSplitMask)
        .value("STATUS_SPLIT_SHIFT", LargeIslandSplitter::Splits::EIterationStatus::StatusSplitShift)
        .value("STATUS_ITEM_MASK", LargeIslandSplitter::Splits::EIterationStatus::StatusItemMask);

    nb::enum_<LargeIslandSplitter::EStatus>(largeIslandSplitterCls, "EStatus",
        "Status code for retrieving a batch")
        .value("WAITING_FOR_BATCH", LargeIslandSplitter::EStatus::WaitingForBatch,
            "Work is expected to be available later")
        .value("BATCH_RETRIEVED", LargeIslandSplitter::EStatus::BatchRetrieved,
            "Work is being returned")
        .value("ALL_BATCHES_DONE", LargeIslandSplitter::EStatus::AllBatchesDone,
            "No further work is expected from this");
}