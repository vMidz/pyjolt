#include "Common.h"
#include <Jolt/ConfigurationString.h>

void BindConfigurationString(nb::module_ &m) {
    m.def("get_configuration_string", &GetConfigurationString,
        "Construct a string that lists the most important configuration settings");
}