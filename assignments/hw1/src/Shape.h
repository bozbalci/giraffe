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

// Class for triangle
class Triangle : public Shape
{
  public:
    Triangle(void); // Constructor
    Triangle(int id, int matIndex, int p1Index, int p2Index,
             int p3Index); // Constructor
    HitRecord intersect(const Ray &ray)
        const; // Will take a ray and return a structure related to the
               // intersection information. You will implement this.

  private:
    // Write any other stuff here
};

// Class for mesh
class Mesh : public Shape
{
  public:
    Mesh(void); // Constructor
    Mesh(int id, int matIndex,
         const std::vector<Triangle> &faces); // Constructor
    HitRecord intersect(const Ray &ray)
        const; // Will take a ray and return a structure related to the
               // intersection information. You will implement this.

  private:
    // Write any other stuff here
};

#endif
