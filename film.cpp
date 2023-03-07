#include <FreeImage.h>
#include "film.h"

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

void Film::draw()
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			glm::vec2 coord = glm::vec2((float)x / (float) w, (float)y / (float) h);
			coord = coord * 2.0f - 1.0f;
			int base = 3 * (x + y * w);
			glm::vec3 color = PerPixel(coord);
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

glm::vec3 Film::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	float discreminent = b * b - 4.0f * a * c;
	
	if (discreminent < 0.0f)
		return glm::vec3(0.0, 0.0, 0.0);

	float t0 = (-b + glm::sqrt(discreminent)) / (2.0f * a);
	float closestT = (-b - glm::sqrt(discreminent)) / (2.0f * a);

	glm::vec3 hitPoint = rayOrigin + rayDirection * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::vec3(-1, -1, -1);
	lightDir = glm::normalize(lightDir);

	float d = glm::max(glm::dot(normal, -lightDir), 0.0f);


	glm::vec3 sphereColor = glm::vec3(1, 0, 1);
	sphereColor *= d;
	return sphereColor;
}
