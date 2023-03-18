#include <FreeImage.h>
#include "film.h"
#include"Camera.h"
#include"Ray.h"
#include"Scene.h"
#include"Intersection.hpp"
#include<stdlib.h>
#include"Object.hpp"
#include"BVH.hpp"
#include"Bbox.hpp"

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

	myActiveScene->buildBVH();

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
		Intersection intersection = TraceRay(ray, myActiveScene->bvh);
		
		if (intersection.hitDistance <= 0.0f)
		{
			glm::vec3 backgroundColor = glm::vec3(0.0f, 0.0f, 0.0f);
			color = backgroundColor;
			break;
		}

		Object* object = intersection.object;

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

Intersection Film::TraceRay(Ray ray, BVHAccel* bvh)
{
	Intersection isect = getIntersection(bvh->root, ray);
	return isect;
}

Intersection Film::ClosestHitSphere(Ray ray, float hitDistance, Object* closestSphere)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.object = closestSphere;

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

Intersection Film::ClosestHitTriangle(Ray ray, float hitDistance, Object* closestTriangle, vec3* vertices)
{
	Intersection intersection;
	intersection.hitDistance = hitDistance;
	intersection.object = closestTriangle;

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

Intersection Film::findIntersection(Ray ray, Object* object)
{
	if (object->type == sphere)
	{
		if (RaySphereIntersect(ray, object).first)
		{
			float t0 = RaySphereIntersect(ray, object).second;
			if (t0 >= 0.000001)
				return ClosestHitSphere(ray, t0, object);
			else
				return Miss(ray);
		}
		else
			return Miss(ray);
	}
	else
	{
		if (RayTriangleIntersect(ray, object, myActiveScene->Vertex).first)
		{
			float t1 = RayTriangleIntersect(ray, object, myActiveScene->Vertex).second;
			if (t1 >= 0.000001)
				return ClosestHitTriangle(ray, t1, object, myActiveScene->Vertex);
			else
				return Miss(ray);
		}
		else
			return Miss(ray);
	}
}

Intersection Film::getIntersection(BVHBuildNode* node, Ray ray)
{
	float x = 0, y = 0, z = 0;
	if (ray.Direction.x != 0.0f) x = 1.0f / ray.Direction.x;
	if (ray.Direction.y != 0.0f) y = 1.0f / ray.Direction.y;
	if (ray.Direction.z != 0.0f) z = 1.0f / ray.Direction.z;
	vec3 invDir(x, y, z);

	std::array<int, 3> dirIsNeg;
	dirIsNeg[0] = ray.Direction.x > 0 ? 0 : 1;
	dirIsNeg[1] = ray.Direction.y > 0 ? 0 : 1;
	dirIsNeg[2] = ray.Direction.z > 0 ? 0 : 1;

	if (!node->bounds.IntersectionP(ray, invDir, dirIsNeg))
	{
		return Miss(ray);
	}


	if (node->left == nullptr && node->right == nullptr)
	{
		return findIntersection(ray, node->object);
	}
		

	Intersection left = getIntersection(node->left, ray);
	Intersection right = getIntersection(node->right, ray);
	if (left.hitDistance > 0 && right.hitDistance > 0)
		return left.hitDistance <= right.hitDistance ? left : right;
	else if (left.hitDistance > 0)
		return left;
	else
		return right;
}
