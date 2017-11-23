#include "RNG.h"

#include <chrono>

// GLOBALS
std::default_random_engine RNG::g_RNG(std::chrono::system_clock::now()
	.time_since_epoch().count());
std::uniform_real_distribution<float> RNG::g_ContinuousDist(0.f, 1.f);
std::uniform_int_distribution<unsigned int> RNG::g_DiscreteDist(0, UINT_MAX);