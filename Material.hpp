#pragma once
class Material
{
public:
	glm::vec3 Albedo{ 1.0f };
	float Roughtness = 1.0f;
	float Metallic = 0.0f;

	Material(glm::vec3 _Albedo, float _Roughtness, float _Metallic): Albedo(_Albedo), Roughtness(_Roughtness), Metallic(_Metallic){}
};
