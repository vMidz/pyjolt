#include "Common.h"
#include "TestFramework/Math/Perlin.h"

void BindPerlin(nb::module_ &m) {
    m.def("perlin_noise3", PerlinNoise3,
        "x"_a, "y"_a, "z"_a, "x_wrap"_a, "y_wrap"_a, "z_wrap"_a);
    m.def("perlin_ridge_noise3", PerlinRidgeNoise3,
        "x"_a, "y"_a, "z"_a, "lacunarity"_a, "gain"_a, "offset"_a, "octaves"_a, "x_wrap"_a, "y_wrap"_a, "z_wrap"_a);
    m.def("perlin_fbm_noise3", PerlinFBMNoise3,
        "x"_a, "y"_a, "z"_a, "lacunarity"_a, "gain"_a, "octaves"_a, "x_wrap"_a, "y_wrap"_a, "z_wrap"_a);
    m.def("perlin_turbulence_noise3", PerlinTurbulenceNoise3,
        "x"_a, "y"_a, "z"_a, "lacunarity"_a, "gain"_a, "octaves"_a, "x_wrap"_a, "y_wrap"_a, "z_wrap"_a);
}