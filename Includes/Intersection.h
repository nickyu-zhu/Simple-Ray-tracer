#pragma once
#include "Ray.h"
#include "Variables.h"

class Intersection
{
public:
	float min_t;
	vec3 hitPoint;
	vec3 normal;
	Ray* ray;
	Object* hitObj;

	Intersection(float _t = INFINITY, vec3 _point = vec3(0.0f), vec3 _normal = vec3(0.0f), Ray* _ray = NULL, Object* _obj = NULL)
	{
		min_t = _t;
		hitPoint = _point;
		normal = _normal;
		ray = _ray;
		hitObj = _obj;
	}
};