#ifndef GZMATHEXT_
#define GZMATHEXT_

#include <math.h>

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef PI
#define PI ((float)3.14159265358979323846264338327950288)
#endif

#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif

template <typename T>
inline
T clamp(T x, T minimum, T maximum)
{
	return max(minimum, min(x, maximum));
}

template <typename T>
inline
T floormin3(T x, T y, T z)
{
	return static_cast<T>(min(floor(x), min(floor(y), floor(z))));
}

template <>
inline
float floormin3<float>(float x, float y, float z)
{
	return min(floorf(x), min(floorf(y), floorf(z)));
}

template <typename T>
inline
T ceilmax3(T x, T y, T z)
{
	return static_cast<T>(max(ceil(x), max(ceil(y), ceil(z))));
}

template <>
inline
float ceilmax3<float>(float x, float y, float z)
{
	return max(ceilf(x), max(ceilf(y), ceilf(z)));
}

#endif
