#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

#include "Scene.h"
#include "Shape.h"

Shape::Shape(void) {}

Shape::Shape(int id, int matIndex) : id(id), matIndex(matIndex) {}

Sphere::Sphere(void) {}

Sphere::Sphere(int id, int matIndex, int cIndex, float R,
               std::vector<vec3f> *vertices)
    : Shape(id, matIndex), centerIdx(cIndex), radius(R), vertices(vertices)
{
}

HitRecord Sphere::intersect(const Ray &ray) const
{
    float a, b, c; // Coefficients of the quadratic equation.
    vec3f sphereCenter = pScene->vertices[centerIdx - 1];

    auto ro_minus_sc = ray.origin - sphereCenter;
    a = ray.direction * ray.direction;
    b = 2.0 * ray.direction * ro_minus_sc;
    c = ro_minus_sc * ro_minus_sc - (radius * radius);

    auto discriminant = b * b - 4 * a * c;
    float t_hit;

    if (discriminant > 0) {
        auto t1 = (-b - std::sqrt(discriminant)) / 2 * a;
        auto t2 = (-b + std::sqrt(discriminant)) / 2 * a;

        if (t1 < 0 && t2 < 0) {
            return NO_HIT;
        } else if (t1 > 0 && t2 > 0) {
            t_hit = std::min(t1, t2);
        } else {
            t_hit = std::max(t1, t2);
        }

        vec3f pos_hit = ray.origin + t_hit * ray.direction;
        vec3f normal_hit = (pos_hit - sphereCenter).normalize();

        return {t_hit, pos_hit, normal_hit, matIndex};
    }

    return NO_HIT;
}

Triangle::Triangle(void) {}

Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index,
                   std::vector<vec3f> *vertices)
    : Shape(id, matIndex), aIdx(p1Index), bIdx(p2Index), cIdx(p3Index),
      vertices(vertices)
{
}

HitRecord Triangle::intersect(const Ray &ray) const
{
    vec3f a = pScene->vertices[aIdx - 1];
    vec3f b = pScene->vertices[bIdx - 1];
    vec3f c = pScene->vertices[cIdx - 1];

    vec3f ab = a - b, ac = a - c;

    float ei_minus_hf = ac.y * ray.direction.z - ray.direction.y * ac.z,
          gf_minus_di = ray.direction.x * ac.z - ac.x * ray.direction.z,
          dh_minus_eg = ac.x * ray.direction.y - ac.y * ray.direction.x,
          j = a.x - ray.origin.x, k = a.y - ray.origin.y,
          l = a.z - ray.origin.z, ak_minus_jb = ab.x * k - j * ab.y,
          jc_minus_al = j * ab.z - ab.x * l, bl_minus_kc = ab.y * l - k * ab.z,
          m = ab.x * (ei_minus_hf) + ab.y * (gf_minus_di) +
              ab.z * (dh_minus_eg),
          t_hit =
              -(ac.z * ak_minus_jb + ac.y * jc_minus_al + ac.x * bl_minus_kc) /
              m;

    if (t_hit <= 0)
        return NO_HIT;

    float gamma =
        (ray.direction.z * ak_minus_jb + ray.direction.y * jc_minus_al +
         ray.direction.x * bl_minus_kc) /
        m;

    if (gamma < 0 || gamma > 1)
        return NO_HIT;

    float beta = (j * ei_minus_hf + k * gf_minus_di + l * dh_minus_eg) / m;

    if (beta < 0 || beta > 1 - gamma)
        return NO_HIT;

    vec3f pos_hit = ray.origin + t_hit * ray.direction;
    vec3f normal_hit = giraffe::cross(b - a, c - a).normalize();

    return {t_hit, pos_hit, normal_hit, matIndex};
}

Mesh::Mesh() {}

Mesh::Mesh(int id, int matIndex, const std::vector<Triangle> &faces,
           std::vector<int> *pIndices, std::vector<vec3f> *vertices)
    : Shape(id, matIndex), faces(faces), pIndices(pIndices), vertices(vertices)
{
    float x_min, y_min, z_min;
    float x_max, y_max, z_max;

    x_min = y_min = z_min = std::numeric_limits<float>::max();
    x_max = y_max = z_max = std::numeric_limits<float>::min();

    for (auto index : *pIndices) {
        vec3f vertex = 0 [vertices][index - 1]; // Art for art's sake

#define IFY(op, a, b)                                                          \
    if ((a)op(b))                                                              \
        b = a;

        IFY(<, vertex.x, x_min);
        IFY(>, vertex.x, x_max);
        IFY(<, vertex.y, y_min);
        IFY(>, vertex.y, y_max);
        IFY(<, vertex.z, z_min);
        IFY(>, vertex.z, z_max);

#undef IFY

        bb_min = {x_min, y_min, z_min};
        bb_max = {x_max, y_max, z_max};
    }
}

HitRecord Mesh::intersect(const Ray &ray) const
{

    float dx = 1 / ray.direction.x;
    float dy = 1 / ray.direction.y;
    float dz = 1 / ray.direction.z;

    float t_x_min, t_x_max, t_y_min, t_y_max, t_z_min, t_z_max;

#define BERK(C)                                                                \
    if ((d##C) >= 0) {                                                         \
        t_##C##_min = (d##C) * (bb_min.C - ray.origin.C);                      \
        t_##C##_max = (d##C) * (bb_max.C - ray.origin.C);                      \
    } else {                                                                   \
        t_##C##_min = (d##C) * (bb_max.C - ray.origin.C);                      \
        t_##C##_max = (d##C) * (bb_min.C - ray.origin.C);                      \
    }

    BERK(x);
    BERK(y);
    BERK(z);

#undef BERK

    float t_min = std::max(std::max(t_x_min, t_y_min), t_z_min);
    float t_max = std::min(std::min(t_x_max, t_y_max), t_z_max);

    if (t_min > t_max)
        return NO_HIT;

    // TODO Rewrite this with std::transform
    t_min = 3.4e+38; // Approximately infinite
    HitRecord hr_min = NO_HIT;

    for (auto face : faces) {
        auto hr = face.intersect(ray);
        auto t_hit = hr.t;

        if (t_hit <= 0)
            continue;

        if (t_hit < t_min) {
            t_min = t_hit;
            hr_min = hr;
        }
    }

    return hr_min;
}
