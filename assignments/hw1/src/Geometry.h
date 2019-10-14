#ifndef GIRAFFE_GEOMETRY_H
#define GIRAFFE_GEOMETRY_H

namespace giraffe
{

struct vec3f {
    union {
        float x;
        float r;
    };
    union {
        float y;
        float g;
    };
    union {
        float z;
        float b;
    };

    float norm() const;
    vec3f normalize() const;
};

// Vector addition and subtraction
vec3f operator-(vec3f vec);
vec3f operator+(vec3f lhs, vec3f rhs);
vec3f operator-(vec3f lhs, vec3f rhs);

// Scalar operations
vec3f operator*(float lhs, vec3f rhs);
vec3f operator/(vec3f lhs, float rhs);

// Dot and cross products
float operator*(vec3f lhs, vec3f rhs);
float dot(vec3f lhs, vec3f rhs);
vec3f cross(vec3f lhs, vec3f rhs);

// Oymak product
vec3f oymak(vec3f lhs, vec3f rhs);

} // namespace giraffe

#endif // GIRAFFE_GEOMETRY_H
