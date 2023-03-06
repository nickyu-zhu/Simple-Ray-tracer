#include"Scene.h"
#include"film.h"

void Scene::render()
{
	Film film = Film(w, h);
	film.draw();
}