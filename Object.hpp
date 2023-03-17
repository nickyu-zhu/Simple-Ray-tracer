#pragma once
#include<iostream>
#include<cmath>
#include<random>
#include"Ray.h"
#include"Bbox.hpp"
#include"Utils.h"
#include"global.hpp"
enum shape { sphere, triangle };

struct Material
{
	glm::vec3 emission;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 ambient;
	float shininess;
};

class Object
{
public:
	shape type;
	glm::vec3 centerPosition{ 0, 0, 0 };
	float Radius;
	glm::mat4 transform;

	int indices[3];

	Material material;
	Bbox getObjectBbox(vec3* Vertex);
};

inline Bbox Object::getObjectBbox(vec3* Vertex)
{
	if (type == triangle)
	{
		return Union(Bbox(Vertex[indices[0]], Vertex[indices[1]]), Vertex[indices[1]]);
	}
	else
	{
		return Bbox(vec3(centerPosition.x - Radius, centerPosition.y - Radius, centerPosition.z - Radius),
			vec3(centerPosition.x + Radius, centerPosition.y + Radius, centerPosition.z + Radius));
	}
}