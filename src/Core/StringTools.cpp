#include "Common.h"
#include <Jolt/Core/StringTools.h>
#include <nanobind/stl/string.h>

void BindStringTools(nb::module_ &m) {
    m.def("convert_to_string_i", &ConvertToString<int>, "value"_a, "Convert type to string");

    m.def("hash_string", &HashString, "string"_a,
        "Calculate the FNV-1a hash of inString.\n"
        "References: \n"
        "    https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function");

    // StringReplace not need for Python

    // Not needed for Python
    // m.def("string_to_vector", [](const char *inString, const char *inDelimiter = ",", bool inClearVector = true) {
    //     Array<String> vector;
    //     nb::typed<nb::list, nb::str> r;
    //     StringToVector(inString, vector, inDelimiter, inClearVector);

    //     for(auto i : vector)
    //         r.append(i.c_str());

    //     return r;
    // }, "string"_a, "delimiter"_a, "clear_vector"_a,
    //    "Convert a delimited string to an array of strings");

    // m.def("VectorToString", [](const Array<String> &inVector, const string_view &inDelimiter = ",") {
    //     String outString;
    //     VectorToString(inVector, outString, inDelimiter);
    // }, "vector"_a, "delimiter"_a, "Convert an array strings to a delimited string");

    // ToLower Not needed for Python

    m.def("nibble_to_binary", &NibbleToBinary, "nibble", "Converts the lower 4 bits of inNibble to a string that represents the number in binary format");
}