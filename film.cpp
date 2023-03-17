#include <FreeImage.h>
#include "film.h"
#include"Camera.h"
#include"Ray.h"
#include"Scene.h"
#include"Intersection.hpp"
#include<stdlib.h>
#include"Object.hpp"

extern int numObjects;
extern int numLights;
extern vec3 attenuation;
extern int maxDepth;
extern const char* outputFilename;

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


void Film::draw(Scene scene, Camera camera)
{
	myActiveCamera = &camera;
	myActiveScene = &scene;

	int printVal = 5;

	int pix = w * h;
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

			int finished = (y * w + x) / (float)pix * 100;
			if (finished % 100 >= printVal) {
				printf("Ray Tracing Progress: %i %%\n", finished);
				printVal += 5;
			}
		}
	}
	FreeImage_Initialise();
	FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	const char* imgName = "raytrace.png";
	//if (outputFilename) imgName = outputFilename;
	FreeImage_Save(FIF_PNG, img, imgName, 0);
	FreeImage_DeInitialise();

}


glm::vec3 Film::RayGen(uint32_t x, uint32_t y)
{
	Ray ray;
	ray.Origin = myActiveCamera->getCamPositon();
	ray.Direction = myActiveCamera->GetRayDirections()[x + y * w];

	glm::vec3 color(0.0f);

	int bounces = 1;
	//bounces = (maxDepth != 0) ? maxDepth : 3;
	for (int i = 0; i < bounces; i++)
	{
		Intersection intersection = TraceRay(ray);
		
		if (intersection.hitDistance < 0.0f)
		{
			glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color = backgroundColor;
			break;
		}

		Object* object = (myActiveScene->Objects[intersection.ObjectIndex]);

		vec3 objDiffuse = object->material.diffuse;
		vec3 objSpecular = object->material.specular;

		for (int i = 0; i < numLights; i++)
		{
			Light* curr_light = &(myActiveScene->lights[i]);
			vec3 lightDir = vec3(0.0f);
			float attnCoeff = 1;

			if (curr_light->lightPosition[3] == 0)
			{
				lightDir = glm::normalize(vec3(curr_light->lightPosition));
				
			}
			else
			{
				lightDir = vec3(curr_light->lightPosition) - intersection.WorldPosition;
				float dist = glm::length(lightDir);
				if (!attenuation.z && !attenuation.x && !attenuation.y) // notice that the attnCoef may be overflow without this line.
					attnCoeff = 1;
				else
					attnCoeff = 1.0f / (attenuation.x + attenuation.y * dist + attenuation.z * dist * dist);
				lightDir = glm::normalize(lightDir);
			}
			vec3 eyedirn = glm::normalize(intersection.WorldPosition - ray.Origin);
			vec3 halfvec = glm::normalize(-eyedirn + lightDir);
			vec3 lightCol = curr_light->lightColor;

			color += attnCoeff * ComputeColor(lightDir, lightCol, intersection.WorldNormal, halfvec, objDiffuse, objSpecular, object->material.shininess);
		}
		
		color += object->material.emission + object->material.ambient;

		ray.Origin = intersection.WorldPosition;
		ray.Direction = glm::reflect(ray.Direction, intersection.WorldNormal);
	}
	
	return color;
}

Intersection Film::TraceRay(Ray ray)
{

	int closestObject = -1, cnt = 0;
	float hitDistance = std::numeric_limits<float>::max();
	float t0 = 0, t1 = 0;
	bool flag = true;
	for (int i = 0; i < numObjects; i++)
	{
		Object* object = myActiveScene->Objects[i];

		if (object->type == sphere)
		{
			if (RaySphereIntersect(ray, object).first)
			{
				t0 = RaySphereIntersect(ray, object).second;

				if (t0 > 0.00001f && t0 < hitDistance)
				{
					hitDistance = t0;
					closestObject = i;
					flag = true;
				}
			}
		}
		else if(object->type == triangle)
		{
			if (RayTriangleIntersect(ray, object, myActiveScene->Vertex).first)
			{
				t1 = RayTriangleIntersect(ray, object, myActiveScene->Vertex).second;

				if (t1 > 0.00001f && t1 < hitDistance)
				{
					hitDistance = t1;
					closestObject = i;
					flag = false;
				}
			}
		}
	}

	if (closestObject < 0)
		return Miss(ray);
	
	if (flag)
		return ClosestHitSphere(ray, hitDistance, closestObject);
	else
		return ClosestHitTriangle(ray, hitDistance, closestObject, myActiveScene->Vertex);
}

Intersection Film::ClosestHitSphere(Ray ray, float hitDistance, int objectIndex)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.ObjectIndex = objectIndex;

	Object* closestSphere = myActiveScene->Objects[objectIndex];

	mat4 invTransf = glm::inverse(closestSphere->transform);
	vec3 oriTransf = vec3(invTransf * vec4(ray.Origin, 1.0f));
	vec3 dirTransf = vec3(invTransf * vec4(ray.Direction, 0.0f));

	glm::vec3 hitPosition = oriTransf + hitDistance * dirTransf;
	vec3 sphereNormalObj = hitPosition - closestSphere->centerPosition;

	vec4 hitPointWorld = closestSphere->transform * vec4(hitPosition, 1.0f);
	vec4 sphereNormalWorld = glm::transpose(invTransf) * vec4(sphereNormalObj, 0.0f);

	intersection.WorldPosition = vec3(hitPointWorld / hitPointWorld.w);
	intersection.WorldNormal = glm::normalize(vec3(sphereNormalWorld));

	return intersection;
}

Intersection Film::ClosestHitTriangle(Ray ray, float hitDistance, int objectIndex, vec3* vertices)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.ObjectIndex = objectIndex;

	Object* closestTriangle = myActiveScene->Objects[objectIndex];

	vec3 A = vec3(closestTriangle->transform * vec4(vertices[closestTriangle->indices[0]], 1));
	vec3 B = vec3(closestTriangle->transform * vec4(vertices[closestTriangle->indices[1]], 1));
	vec3 C = vec3(closestTriangle->transform * vec4(vertices[closestTriangle->indices[2]], 1));

	vec3 triNormal = glm::normalize(glm::cross(B - A, C - A));
	vec3 P = ray.Origin + hitDistance * ray.Direction;

	intersection.WorldPosition = P;
	intersection.WorldNormal = triNormal;

	return intersection;
}

Intersection Film::Miss(Ray ray)
{
	Intersection intersection;
	intersection.hitDistance = -1.0f;
	return intersection;
}


