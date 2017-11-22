#include "stdafx.h"

#include <chrono>

#include "Shooting.h"
#include "FormFactor.h"

void Shooting::Perform(EmissionQueue& emitting, std::vector<Triangle>& triList)
{
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float> fsec;

	auto startTime = clock::now();
	auto elapsed = std::chrono::duration_cast<fsec>(clock::now() - startTime);

	while (!emitting.empty() && elapsed.count() < MAX_RUN_TIME)
	{
		Triangle next = emitting.top();
		emitting.pop();
		PerformOne(&next, triList, emitting);

		elapsed = std::chrono::duration_cast<fsec>(clock::now() - startTime);
	}
}

void Shooting::PerformOne(Triangle* src, std::vector<Triangle>& others, EmissionQueue& lit)
{
	auto end = others.end();
	for (auto iter = others.begin(); iter != end; ++iter)
	{
		if (iter->Id != src->Id)
		{
			float formFactor = 0.f;//static_cast<float>(FormFactorCalculator::inst()->LookUp(src->Id, iter->Id));
			float FdA = formFactor * src->Area() / iter->Area();

			Vec3 dBi(iter->reflectance.r * src->emission.r * FdA,
					 iter->reflectance.g * src->emission.g * FdA,
					 iter->reflectance.b * src->emission.b * FdA);

			iter->radiosity += dBi;
			iter->emission += dBi;

			if (length2(iter->emission) >= MIN_EMISSION_THRESHOLD * MIN_EMISSION_THRESHOLD)
			{
				Triangle t = *iter;
				lit.push(t);
				iter->emission = Vec3();
			}
		}
	}
}