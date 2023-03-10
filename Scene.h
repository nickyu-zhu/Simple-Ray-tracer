#pragma once
#include"Ray.h"
#include"Camera.h"
#include<vector>
#include"Sphere.hpp"
#include"Material.hpp"
#include<glm/glm.hpp>

class Scene 
{
public:
	int w = 540;
	int h = 540;
	std::vector<Sphere> Spheres;
	std::vector<Material> Materials;

	Scene(int _w, int _h): w(_w), h(_h){
		Spheres = {};
		Materials = {};
	}

	void addSphere(glm::vec3 position, float radius, int MaterialIndex);
	void addMaterial(Material material);
};