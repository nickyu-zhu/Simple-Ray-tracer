#include"Scene.h"
#include"Camera.h"
#include"Sphere.hpp"
#include"Material.hpp"
#include<glm/glm.hpp>

void Scene::addSphere(glm::vec3 position, float radius, int MaterialIndex)
{
	Sphere sphere(position, radius, MaterialIndex);
	Spheres.push_back(sphere);
}

void Scene::addMaterial(Material _material)
{
	Materials.push_back(_material);
}