#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Vec3.h"
#include "Matrix4.h"

#include <string>

class Camera
{
  public:
    int cameraId;
    Vec3 pos;
    Vec3 gaze;
    Vec3 u;
    Vec3 v;
    Vec3 w;
    double left, right, bottom, top;
    double near;
    double far;
    int horRes;
    int verRes;
    std::string outputFileName;

    Camera();

    Camera(int cameraId, Vec3 pos, Vec3 gaze, Vec3 u, Vec3 v, Vec3 w,
           double left, double right, double bottom, double top, double near,
           double far, int horRes, int verRes, std::string outputFileName);

    Camera(const Camera &other);

    friend std::ostream &operator<<(std::ostream &os, const Camera &c);

    Matrix4 getTransformationMatrix() const;
    Matrix4 getOrtographicMatrix() const;
    Matrix4 getPerspective2OrtographicMatrix() const;
    Matrix4 getPerspectiveProjectionMatrix() const;
    Matrix4 getViewportMatrix() const;
    Matrix4 getViewingMatrix() const;
};

#endif
