#pragma once

#include<FreeImage.h>

class Film {
private:
	int w, h;
	BYTE* pixels;

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