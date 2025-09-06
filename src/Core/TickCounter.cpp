#include "Common.h"
#include <Jolt/Core/TickCounter.h>

void BindTickCounter(nb::module_ &m) {
    m.def("get_processor_tick_count", &GetProcessorTickCount);
}