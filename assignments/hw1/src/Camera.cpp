#include "Camera.h"

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
