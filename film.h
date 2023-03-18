#pragma once

#include<FreeImage.h>
#include<glm/glm.hpp>
#include"Camera.h"
#include"Ray.h"
#include"Scene.h"
#include"Intersection.hpp"
#include"BVH.hpp"

class Film {
private:
	int w, h;
	BYTE* pixels;

	Scene* myActiveScene = nullptr;
	Camera* myActiveCamera = nullptr;

	glm::vec3 RayGen(uint32_t x, uint32_t y);

	Intersection TraceRay(Ray ray, BVHAccel* root);
	Intersection ClosestHitSphere(Ray ray, float hitDistance, Object* closestSphere);
	Intersection ClosestHitTriangle(Ray ray, float hitDistance, Object* closestTriangle, vec3* vertices);
	Intersection Miss(Ray ray);

	Intersection findIntersection(Ray ray, Object* object);
	Intersection getIntersection(BVHBuildNode* node, Ray ray);
public:
	Film(int _w, int _h){
		w = _w, h = _h;
		pixels = new BYTE[3 * w * h];
	}

	~Film() {
		delete[] pixels;
	}

	void draw(Scene scene, Camera camera);
};