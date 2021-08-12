#include "lak/math.hpp"

#include <cfloat>
#include <cmath>
#include <limits>

bool lak::close_to(double a, double b, uint32_t epsilon_count)
{
	const double diff = std::abs(a - b);
	return (diff < DBL_MIN) ||
	       (diff <= (DBL_EPSILON * std::max(std::abs(a) + std::abs(b), 1.0) *
	                 epsilon_count));
}

bool lak::close_to(float a, float b, uint32_t epsilon_count)
{
	const float diff = std::abs(a - b);
	return (diff < FLT_MIN) ||
	       (diff <= (FLT_EPSILON * std::max(std::abs(a) + std::abs(b), 1.0f) *
	                 epsilon_count));
}
