#ifndef _SHAPE_H_
#define _SHAPE_H_

#include "Ray.h"
#include "defs.h"
#include <vector>

struct Box {
    Box(vec3f min_point, vec3f max_point);
    Box();
    Box(const Box &left, const Box &right);
    void update(const vec3f &p);
    bool intersects(const Ray &ray) const;
    vec3f min_point, max_point;
};

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
};

class BVH : public Shape
{
  public:
    BVH() = default;
    BVH(std::vector<vec3f> *vertices, const std::vector<Triangle> &triangles,
        int axisIndex);
    HitRecord intersect(const Ray &ray) const;

    Box bounding_box;
    Shape *left, *right;
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
    BVH bvh;
};

Box bbox_triangle(std::vector<vec3f> *vertices, const Triangle *triangle);

#endif
