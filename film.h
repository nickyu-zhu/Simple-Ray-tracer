#pragma once

#include<FreeImage.h>
#include<glm/glm.hpp>
#include"Camera.h"
#include"Ray.h"
#include"Scene.h"
#include"Intersection.hpp"

class Film {
private:
	int w, h;
	BYTE* pixels;

	Scene* myActiveScene = nullptr;
	Camera* myActiveCamera = nullptr;

	glm::vec3 RayGen(uint32_t x, uint32_t y);

	Intersection TraceRay(Ray ray);
	Intersection ClosestHit(Ray ray, float hitDistance, int objectIndex);
	Intersection Miss(Ray ray);

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