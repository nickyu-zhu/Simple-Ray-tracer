#pragma once
#include "Utils.h"

// Global variables
// Since all files need access to it, we define EXTERN as either blank or extern,
// depending on if included in the main program or not.  

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

#ifdef MAINPROGRAM 
/** General **/
int w = 500, h = 500; // size
int maxDepth = 5;
/** Camera **/
vec3 center(0.0, 0.0, 0.0); // Center look at point 
float fovy = 90.0; // For field of view
/** Lights **/
vec3 attenuation(1.0, 0.0, 0.0);
/** Materials **/
GLfloat ambient[4] = {0.2, 0.2, 0.2, 1.0};
#else 
/** General **/
EXTERN int w, h; // size
EXTERN int maxDepth;
/** Camera **/
EXTERN vec3 center;
EXTERN float fovy;
/** Lights **/
EXTERN vec3 attenuation;
/** Materials **/
EXTERN GLfloat ambient[4]; // global ambient
#endif 


/** General **/
const char* outputFilename;

/** Camera **/
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 

/** Geometry **/
// shape
enum shape { sphere, triangle };
// vertices
const int maxNumVertices = 100000;
EXTERN vec3 vertices[maxNumVertices];
EXTERN int numVertices;

/** Lights **/ 
// identifying directional & point depends on the 4th dimention of position
const int maxNumLights = 10;
EXTERN GLfloat lightPos[4 * maxNumLights]; // Light Positions/Directions
EXTERN GLfloat lightColor[4 * maxNumLights]; // Light Colors
EXTERN int numLights;                     // How many lights are used 

/** Materials **/
EXTERN GLfloat diffuse[4];
EXTERN GLfloat specular[4];
EXTERN GLfloat emission[4];
EXTERN GLfloat shininess;


// For multiple objects, read from a file.  
const int maxNumObjects = 100000;
EXTERN int numObjects;
EXTERN struct Object {
	shape type;
	GLfloat radius; // radius of the obj if it's a sphere
	vec3 center; // center of the obj if it's a sphere
	int indices[3];
	mat4 transform;

	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat emission[4];
	GLfloat shininess;
} objects[maxNumObjects];