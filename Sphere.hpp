#pragma once
#include<glm/glm.hpp>
#include"Material.hpp"

class Sphere
{
public:
	glm::vec3 Position{ 0, 0, 0 };
	float Radius;

	int MaterialIndex = 0;

	Sphere(glm::vec3& _Position, float _Radius, int _MaterialIndex): Position(_Position), Radius(_Radius), MaterialIndex(_MaterialIndex){}
};