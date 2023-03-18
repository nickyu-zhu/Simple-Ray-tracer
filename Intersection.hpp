#pragma once
#include<glm/glm.hpp>
#include"Object.hpp"

class Intersection
{
public:
	float hitDistance;
	glm::vec3 WorldPosition;
	glm::vec3 WorldNormal;

	Object* object;
};