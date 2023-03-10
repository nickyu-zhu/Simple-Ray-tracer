#ifndef RAY_H
#define RAY_H
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;

class Ray
{
public:
	vec3 Origin;
	vec3 Direction;
};
#endif // !RAY_H


