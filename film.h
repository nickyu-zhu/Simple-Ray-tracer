#pragma once

#include<FreeImage.h>
#include<glm/glm.hpp>

class Film {
private:
	int w, h;
	BYTE* pixels;

	glm::vec3 PerPixel(glm::vec2 coord);

public:
	Film(int _w, int _h){
		w = _w, h = _h;
		pixels = new BYTE[3 * w * h];
	}

	~Film() {
		delete[] pixels;
	}

	void draw();
};