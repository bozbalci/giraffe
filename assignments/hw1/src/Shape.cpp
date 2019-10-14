#include <algorithm>
#include <cmath>
#include <cstdio>

#include "Scene.h"
#include "Shape.h"

Shape::Shape(void) {}

Shape::Shape(int id, int matIndex) : id(id), matIndex(matIndex) {}

Sphere::Sphere(void) {}

Sphere::Sphere(int id, int matIndex, int cIndex, float R) : Shape(id, matIndex)
{
    centerIdx = cIndex;
    radius = R;
}

HitRecord Sphere::intersect(const Ray &ray) const
{
    float a, b, c; // Coefficients of the quadratic equation.
    vec3f sphereCenter = pScene->vertices[centerIdx - 1];

    a = ray.direction * ray.direction;
    b = 2.0 * ray.direction * (ray.origin - sphereCenter);
    c = (ray.origin - sphereCenter) * (ray.origin - sphereCenter) -
        (radius * radius);

    auto discriminant = b * b - 4 * a * c;
    float t_hit;

    if (discriminant > 0) {
        auto t1 = (-b - std::sqrt(discriminant)) / 2 * a;
        auto t2 = (-b + std::sqrt(discriminant)) / 2 * a;

        if (t1 < 0 && t2 < 0) {
            goto no_hit;
        } else if (t1 > 0 && t2 > 0) {
            t_hit = std::min(t1, t2);
        } else {
            t_hit = std::max(t1, t2);
        }

        vec3f pos_hit = ray.origin + t_hit * ray.direction;
        vec3f normal_hit = (pos_hit - sphereCenter).normalize();

        return {true, pos_hit, normal_hit, matIndex};
    }

no_hit:
    return {false, {0, 0, 0}, {0, 0, 0}, -1};
}

Triangle::Triangle(void) {}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index)
    : Shape(id, matIndex)
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
}

/* Triangle-ray intersection routine. You will implement this.
Note that HitRecord structure should hold the information related to the
intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in HitRecord structure you think you will
need. It is in defs.h file. */
HitRecord Triangle::intersect(const Ray &ray) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
}

Mesh::Mesh() {}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const std::vector<Triangle> &faces)
    : Shape(id, matIndex)
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
}

/* Mesh-ray intersection routine. You will implement this.
Note that HitRecord structure should hold the information related to the
intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in HitRecord structure you think you will
need. It is in defs.h file. */
HitRecord Mesh::intersect(const Ray &ray) const
{
    /***********************************************
     *                                             *
     * TODO: Implement this function               *
     *                                             *
     ***********************************************
     */
}
