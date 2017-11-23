#include "RNG.h"

#include <chrono>

// GLOBALS
std::minstd_rand0 RNG::g_RNG(std::chrono::system_clock::now()
	.time_since_epoch().count());