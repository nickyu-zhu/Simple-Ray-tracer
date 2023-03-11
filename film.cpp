#include <FreeImage.h>
#include "film.h"
#include"Camera.h"
#include"Ray.h"
#include"global.hpp"
#include"Scene.h"
#include"Intersection.hpp"
#include<stdlib.h>

namespace Utils {
	static uint32_t ConvertToRGB(const glm::vec3& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);

		uint32_t result = (b << 16) | (g << 8) | r;
		return result;
	}
}

Intersection Film::TraceRay(Ray ray)
{

	int closestSphere = -1;
	float hitDistance = std::numeric_limits<float>::max();
	for (size_t i = 0;  i < myActiveScene->Spheres.size(); i++)
	{
		Sphere sphere = myActiveScene->Spheres[i];
		glm::vec3 origin = ray.Origin - sphere.Position;

		float a = glm::dot(ray.Direction, ray.Direction);
		float b = 2.0f * glm::dot(origin, ray.Direction);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;
		float t0, t1;

		if (!solveQuadratic(a, b, c, t0, t1))
			continue;

		if (t0 > 0.0f && t0 < hitDistance)
		{
			hitDistance = t0;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0)
		return Miss(ray);

	return ClosestHit(ray, hitDistance, closestSphere);

	
}

void Film::draw(Scene scene, Camera camera)
{
	myActiveCamera = &camera;
	myActiveScene = &scene;

	
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			int base = 3 * (x + y * w);
			glm::vec3 color = RayGen(x, y);
			color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
			uint32_t result_color = Utils::ConvertToRGB(color);
			
			pixels[base] = (uint8_t)(result_color >> 16);
			pixels[base + 1] = (uint8_t)(result_color >> 8);
			pixels[base + 2] = (uint8_t)result_color;

		}
	}
	FreeImage_Initialise();
	FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	FreeImage_Save(FIF_PNG, img, "raytrace.png", 0);
	FreeImage_DeInitialise();
}


glm::vec3 Film::RayGen(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = myActiveCamera->getCamPositon();
	ray.Direction = myActiveCamera->GetRayDirections()[x + y * w];

	glm::vec3 color(0.0f);
	float multiplier = 1.0f;

	int bounces = 8;
	for (int i = 0; i < bounces; i++)
	{
		Intersection intersection = TraceRay(ray);
		if (intersection.hitDistance < 0.0f)
		{
			glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
			color += skyColor * multiplier;
			break;
		}

		glm::vec3 lightDir = glm::vec3(-1, -1, -1);
		lightDir = glm::normalize(lightDir);

		float d = glm::max(glm::dot(intersection.WorldNormal, -lightDir), 0.0f);

		Sphere& sphere = myActiveScene->Spheres[intersection.ObjectIndex];
		Material& material = myActiveScene->Materials[sphere.MaterialIndex];

		glm::vec3 sphereColor = material.emission;
		sphereColor *= d;
		color += sphereColor * multiplier;

		multiplier *= 0.7f;

		ray.Origin = intersection.WorldPosition + intersection.WorldNormal * 0.0001f;
		float random = (rand() % 100) / 100 * 0.5f - 0.5f;
		ray.Direction = glm::reflect(ray.Direction, intersection.WorldNormal + material.diffuse * random);
	}
	
	return color;
}

Intersection Film::ClosestHit(Ray ray, float hitDistance, int objectIndex)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.ObjectIndex = objectIndex;

	Sphere& closestSphere = myActiveScene->Spheres[objectIndex];

	glm::vec3 origin = ray.Origin - closestSphere.Position;
	intersection.WorldPosition = origin + ray.Direction * hitDistance;
	intersection.WorldNormal = glm::normalize(intersection.WorldPosition);

	intersection.WorldPosition += closestSphere.Position;

	return intersection;
}

Intersection Film::Miss(Ray ray)
{
	Intersection intersection;
	intersection.hitDistance = -1.0f;
	return intersection;
}


