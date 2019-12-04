#include "Camera.h"
#include "Helpers.h"
#include "Scene.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

extern Scene *scene;

Camera::Camera(int cameraId, Vec3 pos, Vec3 gaze, Vec3 u, Vec3 v, Vec3 w,
               double left, double right, double bottom, double top,
               double near, double far, int horRes, int verRes,
               std::string outputFileName)
{

    this->cameraId = cameraId;
    this->pos = pos;
    this->gaze = gaze;
    this->u = u;
    this->v = v;
    this->w = w;
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    this->near = near;
    this->far = far;
    this->horRes = horRes;
    this->verRes = verRes;
    this->outputFileName = outputFileName;
}

Camera::Camera(const Camera &other)
{
    this->cameraId = other.cameraId;
    this->pos = other.pos;
    this->gaze = other.gaze;
    this->u = other.u;
    this->v = other.v;
    this->w = other.w;
    this->left = other.left;
    this->right = other.right;
    this->bottom = other.bottom;
    this->top = other.top;
    this->near = other.near;
    this->far = other.far;
    this->horRes = other.horRes;
    this->verRes = other.verRes;
    this->outputFileName = other.outputFileName;
}

std::ostream &operator<<(std::ostream &os, const Camera &c)
{
    os << std::fixed << std::setprecision(6) << "Camera " << c.cameraId
       << " => pos: " << c.pos << " gaze: " << c.gaze << std::endl
       << "\tu: " << c.u << " v: " << c.v << " w: " << c.w << std::endl
       << std::fixed << std::setprecision(3) << "\tleft: " << c.left
       << " right: " << c.right << " bottom: " << c.bottom << " top: " << c.top
       << std::endl
       << "\tnear: " << c.near << " far: " << c.far
       << " resolutions: " << c.horRes << "x" << c.verRes
       << " fileName: " << c.outputFileName;

    return os;
}

Matrix4 Camera::getTransformationMatrix() const {
    double mat[4][4] = {
        {u.x, u.y, u.z, -(u.x * pos.x + u.y * pos.y + u.z * pos.z)},
        {v.x, v.y, v.z, -(v.x * pos.x + v.y * pos.y + v.z * pos.z)},
        {w.x, w.y, w.z, -(w.x * pos.x + w.y * pos.y + w.z * pos.z)},
        {0., 0., 0., 1.}
    };

    return mat;
}

Matrix4 Camera::getOrtographicMatrix() const {
    double mat[4][4] = {
        {2./(right - left), 0., 0., -(right + left)/(right - left)},
        {0., 2./(top -bottom), 0., -(top + bottom)/(top - bottom)},
        {0., 0., -2./(far - near), -(far + near)/(far - near)},
        {0., 0., 0., 1.}
    };

    return mat;
}

Matrix4 Camera::getPerspective2OrtographicMatrix() const {
    double mat[4][4] = {
        {near, 0., 0., 0.},
        {0., near, 0., 0.},
        {0., 0., far + near, far * near},
        {0., 0., -1., 0.}
    };

    return mat;
}

Matrix4 Camera::getPerspectiveProjectionMatrix() const {
    return multiplyMatrixWithMatrix(
        getOrtographicMatrix(),
        getPerspective2OrtographicMatrix()
    );
}

Matrix4 Camera::getViewportMatrix() const {
    double mat[4][4] = {
        {horRes/2., 0., 0., (horRes - 1.)/2.},
        {0., verRes/2., 0., (verRes - 1.)/2.},
        {0., 0., .5, .5},
        {0., 0., 0., 1.}
    };

    return mat;
}

Matrix4 Camera::getViewingMatrix() const {
    if (scene->projectionType == 0) {
        return multiplyMatrixWithMatrix(
            getOrtographicMatrix(),
            getTransformationMatrix()
        );
    } else if (scene->projectionType == 1) {
        return multiplyMatrixWithMatrix(
            getPerspectiveProjectionMatrix(),
            getTransformationMatrix()
        );
    } else {
        std::cerr << "Invalid projection type supplied.\n";
        std::exit(1);
    }
}
