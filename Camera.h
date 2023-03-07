#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"Ray.h"
#include"Sample.h"

const float pi = 3.14159265;

class Camera
{
private:
	glm::vec3 camPosition;
	glm::vec3 camCenter;
	glm::vec3 upVector;
	float fov;

	Camera(float lookfromx, float lookfromy, float lookfromz, float lookatx, float lookaty, float lookatz, float upx, float upy, float upz, float _fov)
	{
		this->camPosition = glm::vec3(lookfromx, lookfromy, lookfromz);
		this->camCenter = glm::vec3(lookatx, lookaty, lookatz);
		this->upVector = glm::vec3(upx, upy, upz);
		this->fov = _fov;
	}

	void generateRay(Sample& sample, Ray& ray);
};