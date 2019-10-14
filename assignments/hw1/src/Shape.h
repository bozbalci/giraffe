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
    Sphere(int id, int matIndex, int cIndex, float R);
    HitRecord intersect(const Ray &ray) const;

  private:
    int centerIdx;
    float radius;
};

class Triangle : public Shape
{
  public:
    Triangle(void);
    Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index);
    HitRecord intersect(const Ray &ray) const;

  private:
    int aIdx, bIdx, cIdx;
};

class Mesh : public Shape
{
  public:
    Mesh(void);
    Mesh(int id, int matIndex, const std::vector<Triangle> &faces);
    HitRecord intersect(const Ray &ray) const;

  private:
    std::vector<Triangle> faces;
};

#endif
