#pragma once
#include "Ray.h"

const float pi = 3.14159265;

class Camera
{
private:
	vec3 eye;
	vec3 center;
	vec3 up;
	vec3 u, v, w;
	float fovy;
	int width, height;

	Camera(vec3 _eye, vec3 _center, vec3 _up, float _fovy)
	{
		eye = _eye;
		center = _center;
		up = _up;
		fovy = _fovy;
		w = glm::normalize(eye - center);
		u = glm::normalize(glm::cross(up, w));
		v = glm::cross(w, u);
	}

	Ray RayThruPixel(int x, int y) 
	{
		// refer to these slides: 
		// https://cseweb.ucsd.edu/~alchern/teaching/cse167_fa21/7-1RayTracing.pdf
		// https://cseweb.ucsd.edu/~viscomp/classes/cse167/wi23/slides/lecture16.pdf
		
		float alpha = tan(pi / 360 * fovy) / height * (2 * x - width);
		float beta = tan(pi / 360 * fovy) / height * (height - 2 * y);
		vec3 dir = glm::normalize(alpha * u + beta * v - w);
		return Ray(eye, dir);
	}
};