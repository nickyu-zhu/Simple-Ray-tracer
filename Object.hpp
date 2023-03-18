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
		vec3 A = vec3(transform * vec4(Vertex[indices[0]], 1));
		vec3 B = vec3(transform * vec4(Vertex[indices[1]], 1));
		vec3 C = vec3(transform * vec4(Vertex[indices[2]], 1));

		return Union(Bbox(A, B), C);
	}
	else
	{
		vec3 Center = vec3(transform * vec4(centerPosition, 1.0f));
		float max_val = std::max(transform[0][0], std::max(transform[1][1], transform[2][2]));
		return Bbox(vec3(Center.x - Radius * max_val, Center.y - Radius * max_val, Center.z - Radius * max_val),
			vec3(Center.x + Radius * max_val, Center.y + Radius * max_val, Center.z + Radius * max_val));
	}
}