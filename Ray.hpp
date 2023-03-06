#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef glm::mat3 mat3;

class Ray {
private:
	vec3 origin;
	vec3 direction;
	double t; //transportation time
	double t_min, t_max;

public:	
	Ray(const vec3& ori, const vec3& dir, const double _t = 0.0) : origin(ori), direction(dir), t(_t) {
		t_min = 0.0;
		t_max = std::numeric_limits<double>::max();
	}

	vec3 current_ray(double _t)
	{
		this->t = _t;
		return this->origin + static_cast<float>(this->t) * this->direction;
	}
};