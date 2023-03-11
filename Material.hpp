#pragma once
class Material
{
public:
	glm::vec3 emission{ 1.0f };
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material(glm::vec3 _emission, glm::vec3 _diffuse, glm::vec3 _specular, float _shininess): emission(_emission), diffuse(_diffuse), specular(_specular), shininess(_shininess){}
};
