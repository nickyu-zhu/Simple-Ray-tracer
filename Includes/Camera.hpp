#pragma once
#include "Ray.h"

class Camera
{
private:
	vec3 eye;
	vec3 center;
	vec3 up;
	vec3 u, v, w;
	float fovy;
	int width, height;
public:
	Camera(vec3 _eye, vec3 _center, vec3 _up, float _fovy, int _w, int _h)
	{
		eye = _eye;
		center = _center;
		up = _up;
		fovy = _fovy;
		w = glm::normalize(eye - center);
		u = glm::normalize(glm::cross(up, w));
		v = glm::cross(w, u);
		width = _w;
		height = _h;
	}

	Ray RayThruPixel(int x, int y) 
	{
		// refer to these slides: 
		// https://cseweb.ucsd.edu/~alchern/teaching/cse167_fa21/7-1RayTracing.pdf
		// https://cseweb.ucsd.edu/~viscomp/classes/cse167/wi23/slides/lecture16.pdf
		
		float alpha = tan(pi * fovy / 360) * (2 * x - width) / height;
		float beta = tan(pi * fovy / 360) * (height - 2 * y) / height;
		vec3 dir = glm::normalize(alpha * u + beta * v - w);
		return Ray(eye, dir);
	}
};