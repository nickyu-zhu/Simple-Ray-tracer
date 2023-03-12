#include "Transform.h"

mat3 Transform::rotate(const float degrees, const vec3& axis)
{
    float cosValue = cos(pi / 180 * (degrees));
    float sinValue = sin(pi / 180 * (degrees));
    mat3 identity = mat3(1.0f);
    vec3 axisUnit = glm::normalize(axis);
    float x = axisUnit.x;
    float y = axisUnit.y;
    float z = axisUnit.z;
    vec3 outProduct_x = vec3(x * x, x * y, x * z);
    vec3 outProduct_y = vec3(x * y, y * y, y * z);
    vec3 outProduct_z = vec3(x * z, y * z, z * z);
    mat3 outProduct = mat3(outProduct_x, outProduct_y, outProduct_z);
    mat3 dual = mat3(vec3(0, z, -y), vec3(-z, 0, x), vec3(y, -x, 0));
    mat3 rotateMtx = cosValue * identity + (1 - cosValue) * outProduct + sinValue * dual;
    return rotateMtx;
}

void Transform::left(float degrees, vec3& eye, vec3& up)
{
    mat3 rotation = Transform::rotate(degrees, up);
    eye = rotation * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up)
{
    vec3 axis = glm::cross(eye, up);
    mat3 rotation = Transform::rotate(degrees, axis);
    eye = rotation * eye;
    up = glm::normalize(glm::cross(axis, eye));
}

mat4 Transform::lookAt(const vec3& eye, const vec3& center, const vec3& up)
{
    vec3 forward = center - eye;
    vec3 forwardUnit = glm::normalize(forward);
    vec3 slide = glm::cross(forwardUnit, up);
    vec3 slideUnit = glm::normalize(slide);
    vec3 newUp = glm::cross(slideUnit, forwardUnit);
    mat4 viewRotate = glm::transpose(mat4(
        vec4(slideUnit, 0),
        vec4(newUp, 0),
        vec4(-forwardUnit, 0),
        vec4(0, 0, 0, 1)));
    mat4 viewTranslate = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(-eye, 1));
    mat4 viewMtx = viewRotate * viewTranslate;
    return viewMtx;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    float d = 1 / tan(pi / 180 * fovy * 0.5f);
    mat4 perspMtx = mat4(
        vec4(d / aspect, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, d, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, -(zFar + zNear) / (zFar - zNear), -1.0f),
        vec4(0.0f, 0.0f, -2 * zFar * zNear / ((zFar - zNear)), 0.0f));
    return perspMtx;
}

mat4 Transform::scale(const float& sx, const float& sy, const float& sz)
{
    mat4 scaleMtx = mat4(
        vec4(sx, 0, 0, 0),
        vec4(0, sy, 0, 0),
        vec4(0, 0, sz, 0),
        vec4(0, 0, 0, 1));
    return scaleMtx;
}

mat4 Transform::translate(const float& tx, const float& ty, const float& tz)
{
    mat4 translateMtx = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(tx, ty, tz, 1));
    return translateMtx;
}

vec3 Transform::upvector(const vec3& up, const vec3& zvec)
{
    vec3 x = glm::cross(up, zvec);
    vec3 y = glm::cross(zvec, x);
    vec3 ret = glm::normalize(y);
    return ret;
}