#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Ray.h"
#include "defs.h"

// Forward declarations to avoid cyclic references
class Camera;
class PointLight;
class Material;
class Shape;

// Class to hold everything related to a scene.
class Scene
{
  public:
    int maxRecursionDepth; // Maximum recursion depth
    float intTestEps;   // IntersectionTestEpsilon. You will need this one while
                        // implementing intersect routines in Shape class
    float shadowRayEps; // ShadowRayEpsilon. You will need this one while
                        // generating shadow rays.
    vec3f backgroundColor; // Background color
    vec3f ambientLight;    // Ambient light radiance

    std::vector<Camera *> cameras;     // Vector holding all cameras
    std::vector<PointLight *> lights;  // Vector holding all point lights
    std::vector<Material *> materials; // Vector holding all materials
    std::vector<vec3f> vertices;  // Vector holding all vertices (vertex data)
    std::vector<Shape *> objects; // Vector holding all shapes

    Scene(const char *xmlPath); // Constructor. Parses XML file and initializes
                                // vectors above. Implemented for you.

    void
    renderScene(void); // Method to render scene, an image is created for each
                       // camera in the scene. You will implement this.

  private:
    // Write any other stuff here
};

#endif
