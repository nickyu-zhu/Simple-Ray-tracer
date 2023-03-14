#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"Ray.h"
#include"Sample.h"
#include<vector>

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
	Camera(glm::vec3 _camPosition, glm::vec3 _center, glm::vec3 _up, float _fov)
	{
		this->camPosition = _camPosition;
		this->camCenter = _center;
		this->upVector = _up;
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