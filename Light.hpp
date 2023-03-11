#pragma once
#include<glm/glm.hpp>

class Light
{
public:
	glm::vec3 lightColor;
};

class DirectionalLight : Light
{
public:
	glm::vec3 lightDirection;

	DirectionalLight(glm::vec3 _lightDirection, glm::vec3 _lightColor)
	{
		this->lightColor = _lightColor;
		this->lightDirection = _lightDirection;
	}
};

class PointLight : Light 
{
public:
	glm::vec3 lightPosition;

	PointLight(glm::vec3 _lightPosition, glm::vec3 _lightColor)
	{
		this->lightPosition = _lightPosition;
		this->lightColor = _lightColor;
	}
};
