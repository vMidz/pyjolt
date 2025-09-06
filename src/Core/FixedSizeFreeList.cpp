#include "Common.h"
#include <Jolt/Core/FixedSizeFreeList.h>
#include <Jolt/Core/JobSystem.h>
#include <Jolt/AABBTree/AABBTreeBuilder.h>
#include <Jolt/Physics/Collision/BroadPhase/QuadTree.h>

template <typename T>
static void BindInst(nb::module_ &m, const char *name) {
    using CurrentInst = FixedSizeFreeList<T>;
    nb::class_<CurrentInst, NonCopyable> fixed_size_cls(m, name,
        "Class that allows lock free creation / destruction of objects (unless a new page of objects needs to be allocated)\n"
        "It contains a fixed pool of objects and also allows batching up a lot of objects to be destroyed\n"
        "and doing the actual free in a single atomic operation");
    fixed_size_cls
        .def_ro_static("cInvalidObjectIndex", &CurrentInst::cInvalidObjectIndex, "Invalid index")
        .def_ro_static("ObjectStorageSize", &CurrentInst::ObjectStorageSize, "Size of an object + bookkeeping for the freelist")

        .def("init", &CurrentInst::Init)
        // TODO:
        // .def("construct_object", &CurrentInst::ConstructObject)
        .def("destruct_object", nb::overload_cast<uint32>(&CurrentInst::DestructObject),
            "Lockless destruct an object and return it to the free pool")
        .def("destruct_object", nb::overload_cast<T *>(&CurrentInst::DestructObject),
            "Lockless destruct an object and return it to the free pool");

    nb::class_<typename CurrentInst::Batch>(fixed_size_cls, "FixedSizeFreeList_Batch")
        .def_rw("mFirstObjectIndex", &CurrentInst::Batch::mFirstObjectIndex)
        .def_rw("mLastObjectIndex", &CurrentInst::Batch::mLastObjectIndex)
        .def_rw("mNumObjects", &CurrentInst::Batch::mNumObjects);

    fixed_size_cls
        .def("add_object_to_batch", &CurrentInst::AddObjectToBatch,
            "batch"_a, "index"_a,
            "Add a object to an existing batch to be destructed.\n"
            "Adding objects to a batch does not destroy or modify the objects, this will merely link them\n"
            "so that the entire batch can be returned to the free list in a single atomic operation")
        .def("destruct_object_batch", &CurrentInst::DestructObjectBatch, "batch"_a, "Lockless destruct batch of objects")
        .def("add_object_to_batch", nb::overload_cast<uint32>(&CurrentInst::Get), "object_index"_a, "Access an object by index.");
}

void BindFixedSizeFreeList(nb::module_ &m) {
    BindInst<AABBTreeBuilder::Node>(m, "FixedSizeFreeListNode");

    // BindInst<QuadTree::Node>(m, "QuadTreeNode");
}