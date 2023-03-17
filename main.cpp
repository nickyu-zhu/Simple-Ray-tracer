#include<iostream>
#include"Scene.h"
#include"Camera.h"
#include"Film.h"
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include<glm/glm.hpp>
#include"Material.hpp"
#include "Transform.h"
#include<chrono>
#include"Object.hpp"

using namespace std;

// Global variables
// Since all files need access to it, we define extern as either blank or extern,
// depending on if included in the main program or not.  

/** General **/
int width, height; // size
int maxDepth;
/** Camera **/
vec3 center;
float fovy;
/** Lights **/
vec3 attenuation;
/** Materials **/
float ambient[4]; // global ambient

/** General **/
const char* outputFilename;

/** Camera **/
vec3 eye; // The (regularly updated) vector coordinates of the eye 
vec3 up;  // The (regularly updated) vector coordinates of the up 

/** Geometry **/
// vertices
const int maxNumVertices = 100000;
vec3 vertices[maxNumVertices];
int numVertices;

/** Lights **/
// identifying directional & point depends on the 4th dimention of position
const int maxNumLights = 10;
int numLights;                     // How many lights are used 

/** Materials **/
float diffuse[3];
float specular[3];
float emission[3];
float shininess;

// For multiple objects, read from a file.  
const int maxNumObjects = 1000;
int numObjects;

bool readvals(stringstream& s, const int numvals, float* values)
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i];
        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }
    return true;
}

void readfile(const char* filename, Object* objects, Light* lights)
{
    string str, cmd, outfile;
    ifstream in;
    in.open(filename);
    if (in.is_open()) {
        stack <mat4> transfstack; // matrix stack to store transforms
        transfstack.push(mat4(1.0));

        getline(in, str);
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) { // Ruled out comment and blank lines 
                stringstream s(str);
                s >> cmd;
                int i;
                float values[10]; // max num of params = 10 (camera)
                bool validinput; // Validity of input 

                // size command
                if (cmd == "size") {
                    validinput = readvals(s, 2, values);
                    if (validinput) {
                        width = (int)values[0]; height = (int)values[1];
                    }
                }
                // camera command
                else if (cmd == "camera") {
                    validinput = readvals(s, 10, values); // 10 values eye:3 cen:3 up:3 fov:1
                    if (validinput) {
                        // eye, lookfrom
                        eye = vec3(values[0], values[1], values[2]);
                        // center, lookat
                        center = vec3(values[3], values[4], values[5]);
                        // up
                        up = glm::normalize(vec3(values[6], values[7], values[8]));
                        up = Transform::upvector(up, center - eye);
                        // fovy
                        fovy = values[9];
                    }
                }
                // output command
                else if (cmd == "output") {
                    s >> outfile;
                    if (s.fail()) {
                        cout << "Failed reading output filename";
                    }
                }
                // maxdepth command
                else if (cmd == "maxdepth") {
                    validinput = readvals(s, 1, values);
                    if (validinput) {
                        maxDepth = values[0];
                    }
                }
                // directional & point command
                else if (cmd == "directional" || cmd == "point") {
                    if (numLights == maxNumLights) { // No more lights 
                        cerr << "Reached Maximum Number of Lights " << maxNumLights << " Will ignore further lights\n";
                    }
                    else {
                        validinput = readvals(s, 6, values); // Position/color for lts.
                        if (validinput) {
                            Light* light = &(lights[numLights]);
                            light->lightColor = glm::vec3(values[3], values[4], values[5]);
                            if (cmd == "directional") light->lightPosition = glm::vec4(values[0], values[1], values[2], 0);
                            else if (cmd == "point") light->lightPosition = glm::vec4(values[0], values[1], values[2], 1);
                            numLights++;
                        }
                    }
                }
                // attenuation
                else if (cmd == "attenuation") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        attenuation = vec3(values[0], values[1], values[2]);
                    }
                }
                // material settings 
                else if (cmd == "ambient") {
                    validinput = readvals(s, 3, values); // colors, only 3 values in the given file
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            ambient[i] = values[i];
                        }
                    }
                }
                else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            diffuse[i] = values[i];
                        }
                    }
                }
                else if (cmd == "specular") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            specular[i] = values[i];
                        }
                    }
                }
                else if (cmd == "emission") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            emission[i] = values[i];
                        }
                    }
                }
                else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values);
                    if (validinput) {
                        shininess = values[0];
                    }
                }
                // vertex commands
                // don't need to deal with maxverts command specifically, 
                // just focus on vertex commands
                else if (cmd == "vertex") {
                    if (numVertices == maxNumVertices) { // No more vertices
                        cerr << "Reached Maximum Number of Vertices " << maxNumVertices << " Will ignore further vertices\n";
                    }
                    else {
                        validinput = readvals(s, 3, values);
                        if (validinput) {
                            vertices[numVertices] = vec3(values[0], values[1], values[2]);
                            numVertices++;
                        }
                    }
                }
                // sphere & tri command
                else if (cmd == "sphere" || cmd == "tri") {
                    if (numObjects == maxNumObjects) { // No more objects 
                        cerr << "Reached Maximum Number of Objects " << maxNumObjects << " Will ignore further objects\n";
                    }
                    else {
                        Object* obj = &(objects[numObjects]);

                        // Set the object's light properties

                        glm::vec3 _emission = { emission[0], emission[1], emission[2] };
                        glm::vec3 _diffuse = { diffuse[0],diffuse[1],diffuse[2] };
                        glm::vec3 _specular = { specular[0], specular[1], specular[2] };
                        glm::vec3 _ambient = { ambient[0], ambient[1], ambient[2] };
                        float _shininess = shininess;

                        obj->material.emission = _emission;
                        obj->material.diffuse = _diffuse;
                        obj->material.specular = _specular;
                        obj->material.ambient = _ambient;
                        obj->material.shininess = shininess;

                        // Set the object's transform
                        obj->transform = transfstack.top();

                        // Set the object's type
                        if (cmd == "sphere") {
                            validinput = readvals(s, 4, values);
                            if (validinput) {
                                obj->type = sphere;
                                obj->centerPosition = vec3(values[0], values[1], values[2]);
                                obj->Radius = values[3];
                            }
                            else {
                                cerr << "ERROR: Failed reading sphere object";
                            }
                        }
                        else if (cmd == "tri") {
                            validinput = readvals(s, 3, values);
                            if (validinput) {
                                obj->type = triangle;
                                obj->indices[0] = values[0];
                                obj->indices[1] = values[1];
                                obj->indices[2] = values[2];
                                obj->centerPosition = vec3(1 / 3, 1 / 3, 1 / 3) * (vertices[obj->indices[0]]
                                    + vertices[obj->indices[1]] + vertices[obj->indices[2]]);
                            }
                            else {
                                cerr << "ERROR: Failed reading triangle object";
                            }
                        }
                        numObjects++;
                    }
                }
                // transformation commands
                else if (cmd == "translate") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        mat4 translateMtx = Transform::translate(values[0], values[1], values[2]);
                        *(&transfstack.top()) = transfstack.top() * translateMtx;
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        mat4 scaleMtx = Transform::scale(values[0], values[1], values[2]);
                        *(&transfstack.top()) = transfstack.top() * scaleMtx;
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s, 4, values);
                    if (validinput) {
                        mat4 rotateMtx = mat4(Transform::rotate(values[3], vec3(values[0], values[1], values[2])));
                        *(&transfstack.top()) = transfstack.top() * rotateMtx;
                    }
                }
                else if (cmd == "pushTransform") transfstack.push(transfstack.top());
                else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) cerr << "Stack has no elements. Cannot Pop\n";
                    else transfstack.pop();
                }
                else if (cmd == "maxverts") {// Do nothing;
                }
                else cerr << "Unknown Command: " << cmd << " Skipping \n";
            }
            getline(in, str);
        }
    }
    else {
        cerr << "Unable to Open Input Data File " << filename << "\n";
        throw 2;
    }
}


int main(int argc, char* argv[])
{
    auto start_time = std::chrono::high_resolution_clock::now();
    Object* objects = new Object[maxNumObjects];
    Light* lights = new Light[maxNumLights];
	readfile(argv[1], objects, lights);

	Scene scene = Scene(width, height);

    for (int i = 0; i < numObjects; i++)
        scene.addObject(&objects[i]);

    scene.Vertex = vertices;
    scene.lights = lights;

	Camera camera(eye, center, up, fovy);
	camera.viewPortresize(scene.w, scene.h);
	camera.calculateRayDirection();
	Film film = Film(scene.w, scene.h);
	film.draw(scene, camera);
    printf("\nRay Tracing Finished!\n Please check the output file!\n");

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "Time taken: " << duration.count() << "seconds" << std::endl;

    delete[] objects;
	cin.get();

	return 0;
}