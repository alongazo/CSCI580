#ifndef _MATH_RNG_H
#define _MATH_RNG_H

#include <random>

// hotfix for min/max compilation issues
#ifdef min
#define MIN_WAS_DEFINED 1
#undef min
#endif

#ifdef max
#define MAX_WAS_DEFINED 1
#undef max
#endif

struct RNG
{
	// UTILITY METHODS
	/**
	* Get a random integer.
	*/
	static unsigned int randDiscrete();

	/**
	* Get a random integer in a range.
	*/
	static unsigned int randDiscrete(unsigned int minVal, unsigned int maxVal);

	/**
	* Get a random continuous number from 0 to 1.
	*/
	static float randContinuous();

	/**
	* Get a random continuous number in the given range.
	*/
	static float randContinuous(float minVal, float maxVal);

	// GLOBALS
	/**
	* The ranom number generator used by the game.
	*/
	static std::minstd_rand0 g_RNG;
};


inline
unsigned int RNG::randDiscrete()
{
	return (g_RNG() - g_RNG.min());
}

inline
unsigned int RNG::randDiscrete(unsigned int minVal, unsigned int maxVal)
{
	return ((g_RNG() - g_RNG.min()) % (maxVal - minVal)) + minVal;
}

inline
float RNG::randContinuous()
{
	return static_cast<float>(g_RNG() - g_RNG.min()) /
		static_cast<float>(g_RNG.max() - g_RNG.min());
}

inline
float RNG::randContinuous(float minVal, float maxVal)
{
	return (static_cast<float>(g_RNG() - g_RNG.min()) /
		static_cast<float>(g_RNG.max() - g_RNG.min())) *
		(maxVal - minVal) + minVal;
}

// hot-fix for compilation issues
#ifdef MIN_WAS_DEFINED
#undef MIN_WAS_DEFINED
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifdef MAX_WAS_DEFINED
#undef MAX_WAS_DEFINED
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#endif