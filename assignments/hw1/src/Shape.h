#ifndef _SHAPE_H_
#define _SHAPE_H_

#include "Ray.h"
#include "defs.h"
#include <vector>

class Shape
{
  public:
    int id;
    int matIndex;

    virtual HitRecord intersect(const Ray &ray) const = 0;

    Shape(void);
    Shape(int id, int matIndex);
};

class Sphere : public Shape
{
  public:
    Sphere(void);
    Sphere(int id, int matIndex, int cIndex, float R,
           std::vector<vec3f> *vertices);
    HitRecord intersect(const Ray &ray) const;

  private:
    int centerIdx;
    float radius;
    std::vector<vec3f> *vertices;
};

class Triangle : public Shape
{
  public:
    Triangle(void);
    Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index,
             std::vector<vec3f> *vertices);
    HitRecord intersect(const Ray &ray) const;

  private:
    int aIdx, bIdx, cIdx;
    std::vector<vec3f> *vertices;
};

class Mesh : public Shape
{
  public:
    Mesh(void);
    Mesh(int id, int matIndex, const std::vector<Triangle> &faces,
         std::vector<int> *pIndices, std::vector<vec3f> *vertices);
    HitRecord intersect(const Ray &ray) const;

  private:
    std::vector<Triangle> faces;
    std::vector<int> *pIndices;
    std::vector<vec3f> *vertices;
};

#endif
