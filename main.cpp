#include<iostream>
#include"Scene.h"
#include"Camera.h"
#include"Film.h"
#include<glm/glm.hpp>
#include"Material.hpp"

int main(int argc, char** argv)
{
	Scene scene = Scene(540, 540);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	float radius = 1.0f;
	glm::vec3 Albedo = { 1.0f, 0.0f, 1.0f };
	Material Mat(Albedo, 0.0f, 0.0f);

	scene.addMaterial(Mat);
	scene.addSphere(position, radius, 0);

	position = glm::vec3(0.0f, -101.4f, 0.0f);
	radius = 100.0f;
	Mat.Albedo = { 0.2f, 0.3f, 1.0f };
	Mat.Roughtness = 0.1f;

	scene.addMaterial(Mat);
	scene.addSphere(position, radius, 1);

	Camera camera(5, 0, 0, 0, 0, 0, 0, 1, 0, 90);
	camera.viewPortresize(scene.w, scene.h);
	camera.calculateRayDirection();
	Film film = Film(scene.w, scene.h);
	film.draw(scene, camera);

	return 0;
}