#include "Camera.h"

#include <vector>

Camera::Camera(int id, const char *imageName, const vec3f &pos,
               const vec3f &gaze, const vec3f &up, const ImagePlane &imgPlane)
    : id(id), imageName(imageName), pos(pos), gaze(gaze), up(up),
      imgPlane(imgPlane)
{
    right = giraffe::cross(gaze, up);
    imageCenter = pos + imgPlane.distance * gaze;
    imageTopLeft = imageCenter + imgPlane.left * right + imgPlane.top * up;
}

Ray Camera::getPrimaryRay(int col, int row) const
{
    // UV coordinates of the requested pixel
    float u = (col + 0.5) * (imgPlane.right - imgPlane.left) / imgPlane.nx;
    float v = (row + 0.5) * (imgPlane.top - imgPlane.bottom) / imgPlane.ny;

    vec3f pixelPos = imageTopLeft + u * right - v * up;
    vec3f origin = pos;
    vec3f direction = (pixelPos - pos).normalize();

    return Ray(origin, direction);
}

std::vector<Ray> Camera::get_rays(int col, int row) const
{
    std::vector<Ray> rays;
    float u, v;
    vec3f pixelPos, origin, direction;

    u = (col + 0.25) * (imgPlane.right - imgPlane.left) / imgPlane.nx;
    v = (row + 0.25) * (imgPlane.top - imgPlane.bottom) / imgPlane.ny;
    pixelPos = imageTopLeft + u * right - v * up;
    origin = pos;
    direction = (pixelPos - pos).normalize();
    rays.push_back(Ray(origin, direction));

    u = (col + 0.75) * (imgPlane.right - imgPlane.left) / imgPlane.nx;
    v = (row + 0.25) * (imgPlane.top - imgPlane.bottom) / imgPlane.ny;
    pixelPos = imageTopLeft + u * right - v * up;
    origin = pos;
    direction = (pixelPos - pos).normalize();
    rays.push_back(Ray(origin, direction));

    u = (col + 0.25) * (imgPlane.right - imgPlane.left) / imgPlane.nx;
    v = (row + 0.75) * (imgPlane.top - imgPlane.bottom) / imgPlane.ny;
    pixelPos = imageTopLeft + u * right - v * up;
    origin = pos;
    direction = (pixelPos - pos).normalize();
    rays.push_back(Ray(origin, direction));

    u = (col + 0.75) * (imgPlane.right - imgPlane.left) / imgPlane.nx;
    v = (row + 0.75) * (imgPlane.top - imgPlane.bottom) / imgPlane.ny;
    pixelPos = imageTopLeft + u * right - v * up;
    origin = pos;
    direction = (pixelPos - pos).normalize();
    rays.push_back(Ray(origin, direction));

    return rays;
}
