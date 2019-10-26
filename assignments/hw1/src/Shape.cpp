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
    bounding_box = Box();

    for (auto index : *pIndices) {
        vec3f vertex = 0 [vertices][index - 1]; // Art for art's sake
        bounding_box.update(vertex);
    }

    bvh = BVH(this->vertices, faces, 0);
}

HitRecord Mesh::intersect(const Ray &ray) const
{
    if (!bounding_box.intersects(ray))
        return NO_HIT;

    return bvh.intersect(ray);
}

Box::Box()
{
    constexpr auto float_max = std::numeric_limits<float>::max(),
                   float_min = std::numeric_limits<float>::lowest();

    min_point = {float_max, float_max, float_max};
    max_point = {float_min, float_min, float_min};
}

Box::Box(vec3f min_point, vec3f max_point)
    : min_point(min_point), max_point(max_point)
{
}

Box::Box(const Box &left, const Box &right) : Box()
{
    update(left.min_point);
    update(left.max_point);
    update(right.min_point);
    update(right.max_point);
}

void Box::update(const vec3f &p)
{
#define IFY(op, a, b)                                                          \
    if ((a)op(b))                                                              \
        b = a;

    IFY(<, p.x, min_point.x);
    IFY(<, p.y, min_point.y);
    IFY(<, p.z, min_point.z);
    IFY(>, p.x, max_point.x);
    IFY(>, p.y, max_point.y);
    IFY(>, p.z, max_point.z);

#undef IFY
}

bool Box::intersects(const Ray &ray) const
{
    float dx = 1 / ray.direction.x, dy = 1 / ray.direction.y,
          dz = 1 / ray.direction.z, t_x_min, t_x_max, t_y_min, t_y_max, t_z_min,
          t_z_max;

#define BERK(C)                                                                \
    if ((d##C) >= 0) {                                                         \
        t_##C##_min = (d##C) * (min_point.C - ray.origin.C);                   \
        t_##C##_max = (d##C) * (max_point.C - ray.origin.C);                   \
    } else {                                                                   \
        t_##C##_min = (d##C) * (max_point.C - ray.origin.C);                   \
        t_##C##_max = (d##C) * (min_point.C - ray.origin.C);                   \
    }

    BERK(x);
    BERK(y);
    BERK(z);

#undef BERK

    float t_min = std::max(std::max(t_x_min, t_y_min), t_z_min),
          t_max = std::min(std::min(t_x_max, t_y_max), t_z_max);

    return t_min <= t_max;
}

BVH::BVH(std::vector<vec3f> *vertices, const std::vector<Triangle> &triangles,
         int axisIndex)
    : Shape(-1, -1)
{
    auto triangle_count = triangles.size();

    if (triangle_count == 0) {
        left = right = nullptr;
    } else if (triangle_count == 1) {
        left = (Shape *)&triangles[0];
        right = nullptr;
        bounding_box = bbox_triangle(vertices, (Triangle *)left);
    } else if (triangle_count == 2) {
        left = (Shape *)&triangles[0];
        right = (Shape *)&triangles[1];
        Box left_bounding_box = bbox_triangle(vertices, (Triangle *)left);
        Box right_bounding_box = bbox_triangle(vertices, (Triangle *)right);
        bounding_box = Box(left_bounding_box, right_bounding_box);
    } else {
        auto half_triangle_count = triangle_count / 2;
        auto triangles_copy = triangles;

        std::sort(triangles_copy.begin(), triangles_copy.end(),
                  [vertices, axisIndex](Triangle s1, Triangle s2) {
                      Box s1_box, s2_box;
                      s1_box = bbox_triangle(vertices, &s1);
                      s2_box = bbox_triangle(vertices, &s2);
                      auto s1_mid_point =
                          (s1_box.min_point + s1_box.max_point) / 2;
                      auto s2_mid_point =
                          (s2_box.min_point + s2_box.max_point) / 2;
                      float s1_mp, s2_mp;
                      if (axisIndex == 0) {
                          s1_mp = s1_mid_point.x;
                          s2_mp = s2_mid_point.x;
                      } else if (axisIndex == 1) {
                          s1_mp = s1_mid_point.y;
                          s2_mp = s2_mid_point.y;
                      } else if (axisIndex == 2) {
                          s1_mp = s1_mid_point.y;
                          s2_mp = s2_mid_point.y;
                      }
                      return s1_mp < s2_mp;
                  });

        std::vector<Triangle> lefts(triangles_copy.begin(),
                                    triangles_copy.begin() +
                                        half_triangle_count);
        std::vector<Triangle> rights(
            triangles_copy.begin() + half_triangle_count, triangles_copy.end());
        auto next_axis = (axisIndex + 1) % 3;
        left = new BVH(vertices, lefts, next_axis);
        auto lefts = new std::vector<Triangle>(triangles_copy.begin(),
                                    triangles_copy.begin() +
                                        half_triangle_count);
        auto rights = new std::vector<Triangle>(
            triangles_copy.begin() + half_triangle_count, triangles_copy.end());
        auto next_axis = (axisIndex + 1) % 3;
        left = new BVH(vertices, *lefts, next_axis);
        right = new BVH(vertices, *rights, next_axis);
    }
}

HitRecord BVH::intersect(const Ray &ray) const
{
    HitRecord right_hr, left_hr;

    if (!bounding_box.intersects(ray))
        return NO_HIT;

    if (left)
        left_hr = left->intersect(ray);
    if (right)
        right_hr = right->intersect(ray);

    if (left_hr.t > 0 && right_hr.t > 0) {
        if (left_hr.t >= right_hr.t)
            return left_hr;
        else
            return right_hr;
    }

    if (left_hr.t > 0)
        return left_hr;
    if (right_hr.t > 0)
        return right_hr;

    return NO_HIT;
}

Box bbox_triangle(std::vector<vec3f> *vertices, Triangle *triangle)
{
    Box bounding_box;

    vec3f a = 0 [vertices][triangle->aIdx - 1];
    vec3f b = 0 [vertices][triangle->bIdx - 1];
    vec3f c = 0 [vertices][triangle->cIdx - 1];

    bounding_box.update(a);
    bounding_box.update(b);
    bounding_box.update(c);

    return bounding_box;
}
