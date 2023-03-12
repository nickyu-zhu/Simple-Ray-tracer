#include "FreeImage.h"
#define MAINPROGRAM 
#include <iostream>
#include <math.h>
#include "Camera.hpp"
#include "Intersection.h"
#include "Readfile.hpp"
/**------------------------------------------- Intersect -------------------------------------------**/

void RaySphereIntersect(Ray* ray, Object* obj, Intersection* hitSection)
{
	// sphere may become ellipsoid after being transformed to the world coord
	// so it's better to detect intersection in the object coord
	// which means we have to transform ray to object coord,
	// i.e., apply the inverse of model transformation to ray
	mat4 invTransf = glm::inverse(obj->transform);
	vec3 oriTransf = vec3(invTransf * vec4(ray->origin, 1.0f));
	vec3 dirTransf = vec3(invTransf * vec4(ray->direction, 0.0f));
	// at^2 + bt + c
	float a = glm::dot(dirTransf, dirTransf);
	float b = 2 * glm::dot(dirTransf, (oriTransf - obj->center));
	float c = glm::dot(oriTransf - obj->center, oriTransf - obj->center) - obj->radius * obj->radius;
	float delta = b * b - 4 * a * c;
	if (delta >= 0) {
		float t1 = (-b + sqrt(delta)) / (2 * a);
		float t2 = (-b - sqrt(delta)) / (2 * a);
		float t = fmin(t1, t2);
		if (0.00001 < t && t < hitSection->min_t) {
			// hit point & sphere normal in obj frame
			vec3 hitPointObj = oriTransf + t * dirTransf;
			vec3 sphereNormalObj = hitPointObj - obj->center;
			// hit point & sphere normal in homogeneous world frame
			vec4 hitPointWorld = obj->transform * vec4(hitPointObj, 1);
			vec4 sphereNormalWorld = glm::transpose(invTransf) * vec4(sphereNormalObj, 0);

			hitSection->min_t = t;
			hitSection->hitPoint = vec3(hitPointWorld / hitPointWorld.w);
			hitSection->normal = glm::normalize(vec3(sphereNormalWorld));
			hitSection->ray = ray;
			hitSection->hitObj = obj;
		}
	}
}

void RayTriangleIntersect(Ray* ray, Object* obj, Intersection* hitSection)
{
	// triangle ABC, point P
	// transformation does not affect interpolation
	// and ray is in world coord, have to transform triangle to world coord
	vec3 A = vec3(obj->transform * vec4(vertices[obj->indices[0]], 1));
	vec3 B = vec3(obj->transform * vec4(vertices[obj->indices[1]], 1));
	vec3 C = vec3(obj->transform * vec4(vertices[obj->indices[2]], 1));

	vec3 triNormal = glm::normalize(glm::cross(C - A, B - A));
	float ray_t = (glm::dot(A, triNormal) - glm::dot(ray->origin, triNormal)) / glm::dot(ray->direction, triNormal);
	vec3 P = ray->origin + ray_t * ray->direction;

	// P in triangle? Barycentric Coord -- triangle area ratio, refer to ravi's lecture 16
	// for beta
	vec3 ACcrossAB = glm::cross(C - A, B - A);
	vec3 ACcrossAP = glm::cross(C - A, P - A);
	// for gamma
	vec3 ABcrossAC = -ACcrossAB;
	vec3 ABcrossAP = glm::cross(B - A, P - A);

	if (glm::dot(ACcrossAB, ACcrossAP) >= 0 && glm::dot(ABcrossAC, ABcrossAP) >= 0) { // beta, gamma >= 0
		float beta = glm::length(ACcrossAP) / glm::length(ACcrossAB);
		float gamma = glm::length(ABcrossAP) / glm::length(ABcrossAC);
		if (beta + gamma <= 1) {
			if (0.00001 < ray_t && ray_t < hitSection->min_t) {
				hitSection->min_t = ray_t;
				hitSection->hitPoint = P;
				hitSection->normal = triNormal;
				hitSection->ray = ray;
				hitSection->hitObj = obj;
			}
		}
	}
}

void FindIntersection(Ray* ray, Intersection* hitSection)
{
	for (int i = 0; i < numObjects; i++) {
		Object* obj = &(objects[i]);
		if (obj->type == sphere) RaySphereIntersect(ray, obj, hitSection);
		if (obj->type == triangle) RayTriangleIntersect(ray, obj, hitSection);
	}
}

/**------------------------------------------- FindColor -------------------------------------------**/

vec4 ComputeColor(vec3 lightDir, vec4 lightCol, vec3 normal, vec3 halfVec, vec4 mydiffuse, vec4 myspecular, float myshininess)
{
	float nDotL = glm::dot(normal, lightDir);
	vec4 lambert = mydiffuse * lightCol * fmax(nDotL, 0.0f);
	float nDotH = glm::dot(normal, halfVec);
	vec4 phong = myspecular * lightCol * pow(fmax(nDotH, 0.0f), myshininess);
	return (lambert + phong);
}

vec3 FindColor(Intersection* hitSection, int currDepth)
{
	vec3 allColor = vec3(0);
	if (hitSection->min_t < INFINITY) {
		if (currDepth == maxDepth) return allColor;
		vec3 rayDir = glm::normalize(hitSection->ray->direction);
		vec3 reflectDir = glm::normalize(rayDir - 2 * glm::dot(hitSection->normal, rayDir) * hitSection->normal);
		vec4 diff_spec_col = vec4(0);
		for (int i = 0; i < numLights; i++) {
			vec3 point2lightDir; // hit point to light
			float visibility = 1;
			float attnCoeff = 1;
			// attenuation & visibility
			if (lightPos[4 * i + 3] == 0) { // directional light
				point2lightDir = glm::normalize(vec3(lightPos[4 * i], lightPos[4 * i + 1], lightPos[4 * i + 2]));
			}
			else { // point light
				point2lightDir = vec3(lightPos[4 * i], lightPos[4 * i + 1], lightPos[4 * i + 2]) - hitSection->hitPoint;
				float dist = glm::length(point2lightDir); // for attenuate
				attnCoeff = 1 / (attenuation.x + attenuation.y * dist + attenuation.z * dist * dist);
				point2lightDir = glm::normalize(point2lightDir);
			}
			Ray point2lightRay(hitSection->hitPoint, point2lightDir);
			Intersection* nextHitSection = new Intersection();
			FindIntersection(&point2lightRay, nextHitSection);
			if (nextHitSection->min_t < INFINITY) visibility = 0;

			// shading
			vec3 halfVec = glm::normalize(-rayDir + point2lightDir);
			vec4 lightCol = vec4(lightColor[4 * i], lightColor[4 * i + 1], lightColor[4 * i + 2], 1);
			vec4 objDiffuse = vec4(hitSection->hitObj->diffuse[0], hitSection->hitObj->diffuse[1], hitSection->hitObj->diffuse[2], hitSection->hitObj->diffuse[3]);
			vec4 objSpecular = vec4(hitSection->hitObj->specular[0], hitSection->hitObj->specular[1], hitSection->hitObj->specular[2], hitSection->hitObj->specular[3]);

			diff_spec_col += visibility * attnCoeff * ComputeColor(rayDir, lightCol, hitSection->normal, halfVec, objDiffuse, objSpecular, hitSection->hitObj->shininess);
		}
		allColor.r = hitSection->hitObj->ambient[0] + hitSection->hitObj->emission[0] + diff_spec_col[0];
		allColor.g = hitSection->hitObj->ambient[1] + hitSection->hitObj->emission[1] + diff_spec_col[1];
		allColor.b = hitSection->hitObj->ambient[2] + hitSection->hitObj->emission[2] + diff_spec_col[2];

		//Ray reflectRay(hitSection->hitPoint, reflectDir);
		//FindIntersection(&reflectRay, hitSection);
		//allColor += FindColor(hitSection, currDepth + 1);
		return allColor;
	}
	else {
		return vec3(0);
	}
}

static uint32_t ConvertToRGB(const glm::vec3& color)
{
	uint8_t r = (uint8_t)(color.r * 255.0f);
	uint8_t g = (uint8_t)(color.g * 255.0f);
	uint8_t b = (uint8_t)(color.b * 255.0f);

	uint32_t result = (b << 16) | (g << 8) | r;
	return result;
}

/**------------------------------------------- main -------------------------------------------**/

int main(int argc, char* argv[])
{
	readfile(argv[1]);

	Camera cam(eye, center, up, fovy, w, h);

	int printVal = 5;

	int pix = w * h;
	BYTE* pixels = new BYTE[3 * pix];
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Ray ray = cam.RayThruPixel(x+0.5f, y+0.5f);
			Intersection* hitSection = new Intersection();
			FindIntersection(&ray, hitSection);
			vec3 color = FindColor(hitSection, 0);
			color = glm::clamp(color, glm::vec3(0.0f), glm::vec3(1.0f));
			uint32_t result_color = ConvertToRGB(color);
			int base = 3 * (x + y * w);
			pixels[base] = (uint8_t)(result_color >> 16);
			pixels[base + 1] = (uint8_t)(result_color >> 8);
			pixels[base + 2] = (uint8_t)result_color;

			// progress bar
			int finished = (y * w + x) / (float)pix * 100;
			if (finished % 100 >= printVal) {
				printf("Ray Tracing Progress: %i %%\n", finished);
				printVal += 5;
			}
		}
	}
	printf("\nRay Tracing Finished!\n Please check the output file!");

	FreeImage_Initialise();
	FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);

	const char* imgName = "RayTraceImg.png";
	if (outputFilename) imgName = outputFilename;
	FreeImage_Save(FIF_PNG, img, imgName, 0);

	delete[] pixels;
	FreeImage_DeInitialise();

	std::cin.get();
	return 0;
}