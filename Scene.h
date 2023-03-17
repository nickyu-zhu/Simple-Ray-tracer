#pragma once
#include"Ray.h"
#include"Camera.h"
#include<vector>
#include"Material.hpp"
#include<glm/glm.hpp>
#include"global.hpp"
#include"Light.hpp"
#include"BVH.hpp"

class Scene 
{
public:
	int w = 540;
	int h = 540;
	std::vector<Object*> Objects;
	vec3* Vertex;
	Light* lights;

	Scene(int _w, int _h): w(_w), h(_h){
	}

	void addObject(Object* obj);

	BVHAccel* bvh;
	void buildBVH();
};


