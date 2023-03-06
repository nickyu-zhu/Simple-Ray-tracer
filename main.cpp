#include<iostream>
#include"Scene.h"

int main(int argc, char** argv)
{
	Scene scene = Scene(540, 540);

	scene.render();

	return 0;
}