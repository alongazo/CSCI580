#ifndef _SHOOTINGALG_H
#define _SHOOTINGALG_H

#include <vector>
#include <list>
#include <queue>

#include "Triangle.h"

// provide overload for the less comparator for the emitance queue
template <>
inline
constexpr bool std::less<Triangle>::operator()(const Triangle& lhs, const Triangle& rhs) const
{
	return length2(lhs.emission) < length2(rhs.emission);
}

// Performs shooting
struct Shooting
{
	// Convenience type
	typedef std::priority_queue<Triangle> EmissionQueue;

	// The minimum emission value to continue shooting.
	static constexpr float MIN_EMISSION_THRESHOLD = 0.1f;

	// The maximum run time in seconds.
	static constexpr float MAX_RUN_TIME = 30.0f;

	// Perform shooting.
	static void Perform(EmissionQueue& emitting, std::vector<Triangle>& triList);

	// Perform shooting iteration for one triangle.
	static void PerformOne(Triangle* src, std::vector<Triangle>& others, EmissionQueue& lit);
};

#endif