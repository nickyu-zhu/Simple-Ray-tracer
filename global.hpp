#pragma once
#pragma once
#include<iostream>
#include<cmath>
#include<random>
#include"Utils.h"

#undef PI
#define PI 3.141592653589793f

extern const float EPSILON;
const float kInfinity = std::numeric_limits<float>::max();

inline bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{

	float discreminent = b * b - 4.0f * a * c;
	if (discreminent < 0) return  false;
	else if (discreminent == 0) x0 = x1 = -0.5f * b / a;
	else
	{
		float q = (b > 0) ? -0.5 * (b + sqrt(discreminent)) : -0.5 * (b - sqrt(discreminent));

		x0 = q / a;
		x1 = c / q;
	}

	if (x0 > x1) std::swap(x0, x1);
	return true;
}

inline float get_random_float()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

	return dist(rng);
}
