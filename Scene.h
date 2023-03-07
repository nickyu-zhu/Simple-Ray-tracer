#pragma once
#include"Ray.h"
#include"film.h"

class Scene 
{
public:
	int w = 540;
	int h = 540;

	Scene(int _w, int _h): w(_w), h(_h){}

	void render();
};