#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"Ray.h"
#include"Sample.h"
#include<vector>

const float pi = 3.14159265;

class Camera
{
private:
	glm::vec3 camPosition;
	glm::vec3 camCenter;
	glm::vec3 upVector;
	float fov;

	int m_ViewportWidth = 0, m_ViewportHeight = 0;
	std::vector<glm::vec3> m_RayDirections;
	

public:
	Camera(float lookfromx, float lookfromy, float lookfromz, float lookatx, float lookaty, float lookatz, float upx, float upy, float upz, float _fov)
	{
		this->camPosition = glm::vec3(lookfromx, lookfromy, lookfromz);
		this->camCenter = glm::vec3(lookatx, lookaty, lookatz);
		this->upVector = glm::vec3(upx, upy, upz);
		this->fov = _fov;
	}

	glm::vec3 getCamPositon() { return camPosition; }
	glm::vec3 getCamCenter() { return camCenter; }
	glm::vec3 getupVector() { return upVector; }
	float getFov() { return fov; }

	void viewPortresize(int w, int h)
	{
		m_ViewportWidth = w;
		m_ViewportHeight = h;
	}

	const std::vector<glm::vec3>& GetRayDirections() const { return m_RayDirections; }

	void calculateRayDirection();

};