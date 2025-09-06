#include "Common.h"
#include <Jolt/Core/HashCombine.h>

size_t PyHashCombine(size_t seed, nb::args args) {
    for (const auto &arg : args) {
        // Handle different types using type checking
        if (nb::isinstance<nb::int_>(arg)) {
            HashCombine(seed, nb::cast<int>(arg));
        } else if (nb::isinstance<nb::str>(arg)) {
            HashCombine(seed, nb::cast<std::string>(arg));
        }
    }
    return seed;
}

void BindHashCombine(nb::module_ &m) {
    m.def("hash_bytes", &HashBytes, "data"_a, "size"_a, "seed"_a = 0xcbf29ce484222325UL);
    m.def("hash_64", &Hash64, "value"_a,
        "A 64 bit hash function by Thomas Wang, Jan 1997\n"
        "See: http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm\n"
        "Args:\n"
        "    value: Value to hash\n"
        "Returns:\n"
        "    int: Hash");

    m.def("hash_combine", &PyHashCombine, "seed"_a, "args"_a,
        "Hash combiner to use a custom struct in an unordered map or set\n"
        "Usage:\n"
        "	struct SomeHashKey\n"
        "	{\n"
        "		std::string key1;\n"
        "		std::string key2;\n"
        "		bool key3;\n"
        "	};\n"
        "	JPH_MAKE_HASHABLE(SomeHashKey, t.key1, t.key2, t.key3)");
}