#pragma once
#include<iostream>
#include<cmath>
#include<random>
#include"Ray.h"
#include"Bbox.hpp"
#include"Utils.h"
#include"Object.hpp"
typedef std::pair<bool, float> PII;


#undef pi
#define pi 3.141592653589793f

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

inline PII RaySphereIntersect(Ray ray, Object* obj)
{
	mat4 invTransf = glm::inverse(obj->transform);
	vec3 oriTransf = vec3(invTransf * vec4(ray.Origin, 1.0f));
	vec3 dirTransf = vec3(invTransf * vec4(ray.Direction, 0.0f));

	float a = glm::dot(dirTransf, dirTransf);
	float b = 2 * glm::dot(dirTransf, (oriTransf - obj->centerPosition));
	float c = glm::dot(oriTransf - obj->centerPosition, oriTransf - obj->centerPosition) - obj->Radius * obj->Radius;
	float delta = b * b - 4 * a * c;
	if (delta >= 0) {
		float t1 = (-b + sqrt(delta)) / (2 * a);
		float t2 = (-b - sqrt(delta)) / (2 * a);
		float t = fmin(t1, t2);
		return { true, t };
	}
	else
		return { false, -1 };
}

inline PII RayTriangleIntersect(Ray ray, Object* obj, vec3* vertices)
{
	vec3 A = vec3(obj->transform * vec4(vertices[obj->indices[0]], 1));
	vec3 B = vec3(obj->transform * vec4(vertices[obj->indices[1]], 1));
	vec3 C = vec3(obj->transform * vec4(vertices[obj->indices[2]], 1));

	vec3 triNormal = glm::normalize(glm::cross(C - A, B - A));
	float t0 = (glm::dot(A, triNormal) - glm::dot(ray.Origin, triNormal)) / glm::dot(ray.Direction, triNormal);
	vec3 P = ray.Origin + t0 * ray.Direction;

	// P in triangle? Barycentric Coord -- triangle area ratio, refer to ravi's lecture 16
	// for beta
	vec3 ACcrossAB = glm::cross(C - A, B - A);
	vec3 ACcrossAP = glm::cross(C - A, P - A);
	// for gamma
	vec3 ABcrossAC = -ACcrossAB;
	vec3 ABcrossAP = glm::cross(B - A, P - A);

	if (glm::dot(ACcrossAB, ACcrossAP) >= 0 && glm::dot(ABcrossAC, ABcrossAP) >= 0) { // beta, gamma >= 0
		float beta = glm::length(ACcrossAP) / glm::length(ACcrossAB);
		float gamma = glm::length(ABcrossAP) / glm::length(ABcrossAC);
		if (beta + gamma <= 1)
			if (t0 > 0)
				return { true, t0 };
	}
	return { false, t0 };
}

inline vec3 ComputeColor(vec3 lightDir, vec3 lightCol, vec3 normal, vec3 halfvec, vec3 mydiffuse, vec3 myspecular, float myshininess)
{
	float nDotL = glm::dot(normal, lightDir);
	vec3 lambert = mydiffuse * lightCol * fmax(nDotL, 0.0f);
	float nDotH = glm::dot(normal, halfvec);
	vec3 phong = myspecular * lightCol * pow(fmax(nDotH, 0.0f), myshininess);
	return (lambert + phong);
}

inline float get_random_float()
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

	return dist(rng);
}
