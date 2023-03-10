#include"Camera.h"
#include<vector>
#include<glm/glm.hpp>

void Camera::calculateRayDirection()
{
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	glm::vec3 w = glm::normalize(camPosition - camCenter);
	glm::vec3 u = glm::cross(upVector, w);
	u = glm::normalize(u);
	glm::vec3 v = glm::cross(w, u);

	float aspect = (float)m_ViewportWidth / (float)m_ViewportHeight;

	for(int y = 0; y < m_ViewportHeight; y++)
		for (int x = 0; x < m_ViewportWidth; x++)
		{
			float radian = glm::radians(fov / 2);
			
			float alpha = glm::tan(radian) / aspect * (x - (m_ViewportWidth / 2)) / (m_ViewportWidth / 2);
			float beta = glm::tan(radian) * (y - (m_ViewportHeight / 2)) / (m_ViewportHeight / 2);

			glm::vec3 direction = alpha * u + beta * v - w;
			direction = glm::normalize(direction);

			m_RayDirections[x + y * m_ViewportWidth] = direction;
		}
}