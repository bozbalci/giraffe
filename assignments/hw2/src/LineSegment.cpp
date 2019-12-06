#include "LineSegment.h"
#include "Scene.h"


extern Scene *scene;


static bool line_visible(double den, double num, double *t_e, double *t_l) {
    if (den > 0) {
        auto t = num / den;
        if (t > *t_l) return false;
        if (t > *t_e) *t_e = t;
    } else if (den < 0) {
        auto t = num / den;
        if (t < *t_e) return false;
        if (t < *t_l) *t_l = t;
    } else if (num > 0) return false;
    return true;
}

void LineSegment::clip() {
    // Clips the line segment from a to b using the Liang-Barsky algorithm.
    auto dx = b.x - a.x;
    auto dy = b.y - a.y;
    auto dz = b.z - a.z;

    constexpr double
        x_min = -1.0,
        x_max = 1.0,
        y_min = -1.0,
        y_max = 1.0,
        z_min = -1.0,
        z_max = 1.0
    ;

    Color color_a = scene->getColorById(a.colorId);
    Color color_b = scene->getColorById(b.colorId);
    Color color_diff = {color_b.r - color_a.r, color_b.g - color_a.g, color_b.b - color_a.b};

    double t_e = 0, t_l = 1;

    visible = false;

    if (line_visible(dx, x_min - a.x, &t_e, &t_l)) {
    if (line_visible(-dx, a.x - x_max, &t_e, &t_l)) {
    if (line_visible(dy, y_min - a.y, &t_e, &t_l)) {
    if (line_visible(-dy, a.y - y_max, &t_e, &t_l)) {
    if (line_visible(dz, z_min - a.z, &t_e, &t_l)) {
    if (line_visible(-dz, a.z - z_max, &t_e, &t_l)) {
        visible = true;

        if (t_l < 1) {
            b.x = a.x + dx * t_l;
            b.y = a.y + dy * t_l;
            b.z = a.z + dz * t_l;

            color_b.r = color_a.r + color_diff.r * t_l;
            color_b.g = color_a.g + color_diff.g * t_l;
            color_b.b = color_a.b + color_diff.b * t_l;

            auto id = -(scene->customColors.size() + 1);
            scene->customColors.push_back(color_b);
            b.colorId = id;
        }
        if (t_e > 0) {
            a.x += dx * t_e;
            a.y += dy * t_e;
            a.z += dz * t_e;

            color_a.r += color_diff.r * t_e;
            color_a.g += color_diff.g * t_e;
            color_a.b += color_diff.b * t_e;

            auto id = -(scene->customColors.size() + 1);
            scene->customColors.push_back(color_a);
            a.colorId = id;
        }
    }}}}}}
}
