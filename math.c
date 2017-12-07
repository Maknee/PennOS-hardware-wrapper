#include "math.h"

double fabs(double val)
{
	return val >= 0.0 ? val : -val;
}

//not correct, but eh
float powf(float base, int exponent)
{
	float val = base;
	base = 1.0f;
	if (exponent == 1)
	{
		return val;
	}

	//assume positive...
	for(; exponent > 1; exponent--)
	{
		base *= val;
	}
	return base;
}
