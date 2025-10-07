#include "Common.h"
#include <Jolt/Core/LockFreeHashMap.h>
#include <nanobind/operators.h>

template <typename T>
auto BindInst(nb::module_ &m, std::string name, const char* doc = "") {
    using CurrentInst = LockFreeHashMap<T, T>;
    nb::class_<CurrentInst, NonCopyable> lockFreeHashMapCls(m, ("LockFreeHashMap" + name).c_str(), doc);
    lockFreeHashMapCls
        .def(nb::init<LFHMAllocator &>(), "allocator"_a)
        .def("init", &CurrentInst::Init, "max_buckets"_a,
            "Initialization.\n"
            "Args:\n"
            "    max_buckets (int): Max amount of buckets to use in the hashmap. Must be power of 2.")
        .def("clear", &CurrentInst::Clear, "Remove all elements.\n"
                                           "Note that this cannot happen simultaneously with adding new elements.")
        .def("get_num_buckets", &CurrentInst::GetNumBuckets, "Get the current amount of buckets that the map is using")
        .def("get_max_buckets", &CurrentInst::GetMaxBuckets, "Get the maximum amount of buckets that this map supports")
        .def("set_num_buckets", &CurrentInst::SetNumBuckets, "num_buckets"_a,
            "Update the number of buckets. This must be done after clearing the map and cannot be done concurrently with any other operations on the map.\n"
            "Note that the number of buckets can never become bigger than the specified max buckets during initialization and that it must be a power of 2.")
        // TODO:
        // .def("create", &CurrentInst::Create, "Insert a new element, returns null if map full.\n"
        //  "Multiple threads can be inserting in the map at the same time.")
        .def("find", &CurrentInst::Find, "key"_a, "key_hash"_a, "Find an element, returns null if not found")
        .def("to_handle", &CurrentInst::ToHandle, "key_value"_a, "Get convert key value pair to uint32 handle")
        .def("from_handle", &CurrentInst::FromHandle, "handle"_a, "Convert uint32 handle back to key and value")
#ifdef JPH_ENABLE_ASSERTS
        .def("get_num_key_values", &CurrentInst::GetNumKeyValues,
            "Get the number of key value pairs that this map currently contains.\n"
            "Available only when asserts are enabled because adding elements creates contention on this atomic and negatively affects performance.")
#endif
        .def("get_all_key_values", &CurrentInst::GetAllKeyValues, "all"_a, "Get all key/value pairs")
        .def("begin", &CurrentInst::begin,
            "Iterate over the map, note that it is not safe to do this in parallel to Clear().\n"
            "It is safe to do this while adding elements to the map, but newly added elements may or may not be returned by the iterator.")
        .def("end", &CurrentInst::end)
#ifdef JPH_DEBUG
        .def("trace_stats", &CurrentInst::TraceStats, "Output stats about this map to the log")
#endif
        ;

    nb::class_<typename CurrentInst::KeyValue>(lockFreeHashMapCls, "KeyValue")
        .def("get_key", &CurrentInst::KeyValue::GetKey)
        .def("get_value", nb::overload_cast<>(&CurrentInst::KeyValue::GetValue));

    nb::class_<typename CurrentInst::Iterator>(lockFreeHashMapCls, "Iterator")
        .def(nb::self == nb::self, "rhs"_a, "Comparison")
        .def(nb::self != nb::self, "rhs"_a, "Comparison")
        .def("__mul__", [](typename CurrentInst::Iterator &self){
            return self.operator*();
        })

        .def_rw("map", &CurrentInst::Iterator::mMap)
        .def_rw("bucket", &CurrentInst::Iterator::mBucket)
        .def_rw("offset", &CurrentInst::Iterator::mOffset);

    return lockFreeHashMapCls;
}

void BindLockFreeHashMap(nb::module_ &m) {
    nb::class_<LFHMAllocator, NonCopyable>(m, "LFHMAllocator", "Allocator for a lock free hash map")
        .def(nb::init<>())
        .def("init", &LFHMAllocator::Init, "object_store_size_bytes"_a,
            "Initialize the allocator.\n"
            "Args:\n"
            "    object_store_size_bytes (int): Number of bytes to reserve for all key value pairs")
        .def("clear", &LFHMAllocator::Clear, "Clear all allocations")
        .def("allocate", &LFHMAllocator::Allocate, "block_size"_a, "begin"_a, "end"_a,
            "Allocate a new block of data.\n"
            "Args:\n"
            "    block_size (int): Size of block to allocate (will potentially return a smaller block if memory is full).\n"
            "    begin (int): Should be the start of the first free byte in current memory block on input, will contain the start of the first free byte in allocated block on return.\n"
            "    end (int): Should be the byte beyond the current memory block on input, will contain the byte beyond the allocated block on return.");
    // TODO:
    // ToOffset
    // FromOffset

    nb::class_<LFHMAllocatorContext, NonCopyable>(m, "LFHMAllocatorContext",
        "Allocator context object for a lock free hash map that allocates a larger memory block at once and hands it out in smaller portions.\n"
        "This avoids contention on the atomic LFHMAllocator::mWriteOffset.")

        .def(nb::init<LFHMAllocator &, uint32>(), "allocator"_a, "block_size"_a, "Construct a new allocator context")
        .def("allocate", &LFHMAllocatorContext::Allocate, "size"_a, "alignment"_a, "write_offset"_a,
            "@brief Allocate data block.\n"
            "Args:\n"
            "    size (int): Size of block to allocate.\n"
            "    alignment (int): Alignment of block to allocate.\n"
            "    write_offset (int): Offset in buffer where block is located\n"
            "Returns:\n"
            "    bool: True if allocation succeeded.");

    auto doc = "Very simple lock free hash map that only allows insertion, retrieval and provides a fixed amount of buckets and fixed storage.\n"
                "Note: This class currently assumes key and value are simple types that need no calls to the destructor.";

    auto cls = BindInst<int>(m, "int", doc);
}