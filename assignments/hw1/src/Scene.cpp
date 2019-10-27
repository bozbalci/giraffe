#include <algorithm>
#include <cmath>
#include <cstddef>
#include <future>
#include <iostream>
#include <limits>
#include <thread>

#include "tinyxml2.h"

#include "Camera.h"
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include "Scene.h"
#include "Shape.h"

using namespace tinyxml2;

Color to_output_color(vec3f color)
{
    return {(unsigned char)std::min(color.r, 255.0f),
            (unsigned char)std::min(color.g, 255.0f),
            (unsigned char)std::min(color.b, 255.0f)};
}

void Scene::render_partial(Image &image, Camera *camera, int u_min,
                           int u_max) const
{
    auto height = camera->imgPlane.ny;

    for (std::size_t i = u_min; i < u_max; ++i) {
        for (std::size_t j = 0; j < height; ++j) {
            Ray ray = camera->getPrimaryRay(i, j);
            vec3f color = ray_color(ray, 0);
            image.setPixelValue(i, j, to_output_color(color));
        }
    }
}

vec3f Scene::ray_color(Ray ray, int depth) const
{
    vec3f color = {0, 0, 0};

    if (depth > maxRecursionDepth)
        return color;

    float t_min = std::numeric_limits<float>::max();
    HitRecord hr_min = NO_HIT;

    for (auto object : objects) {
        HitRecord hr = object->intersect(ray);
        auto t_hit = hr.t;

        if (t_hit > 0 && t_hit < t_min) {
            t_min = t_hit;
            hr_min = hr;
        }
    }

    if (hr_min.t > 0) {
        // Viewing ray intersected with an object.
        Material *material = materials[hr_min.materialIdx - 1];

        if (material->mirrorRef.norm() > 0) {
            vec3f reflection_vector =
                ray.direction -
                2.0f * (hr_min.normal * ray.direction) * hr_min.normal;
            Ray reflection_ray(hr_min.pos + shadowRayEps * reflection_vector,
                               reflection_vector.normalize());

            // Mirror component
            vec3f mirror = giraffe::oymak(material->mirrorRef,
                                          ray_color(reflection_ray, depth + 1));
            color = color + mirror;
        }

        // Ambient shading
        vec3f ambient = giraffe::oymak(ambientLight, material->ambientRef);
        color = color + ambient;

        for (auto light : lights) {
            vec3f light_vector = light->position - hr_min.pos;
            float light_distance = light_vector.norm();
            vec3f light_contribution =
                light->computeLightContribution(hr_min.pos);
            Ray light_ray(hr_min.pos + shadowRayEps * light_vector.normalize(),
                          light_vector.normalize());

            // Shadow computation
            bool in_shadow = std::any_of(
                objects.cbegin(), objects.cend(),
                [&light_ray, &light_distance](auto object) {
                    auto hr_shadow = object->intersect(light_ray);
                    return hr_shadow.t > 0 && hr_shadow.t <= light_distance;
                });
            if (in_shadow)
                continue;

            // Diffuse component
            vec3f diffuse =
                std::max(0.0f, hr_min.normal * light_vector.normalize()) *
                giraffe::oymak(material->diffuseRef, light_contribution);
            color = color + diffuse;

            // Specular component (Blinn-Phong)
            vec3f h = -ray.direction.normalize() + light_vector.normalize();
            h = h / h.norm();
            vec3f specular =
                std::pow(std::max(0.0f, hr_min.normal * h),
                         material->phongExp) *
                giraffe::oymak(material->specularRef, light_contribution);
            color = color + specular;
        }

        return color;
    }

    return backgroundColor;
}

void Scene::renderScene(void)
{
    for (auto camera : cameras) {
        auto width = camera->imgPlane.nx;
        auto height = camera->imgPlane.ny;

        Image image(width, height);

        // std::thread::hardware_concurrency returns zero when the value is not
        // well defined or computable, so we force the rendering to run in a
        // single thread in that case.
        const unsigned int num_threads =
            std::max(std::thread::hardware_concurrency(), 1u);

        const int stride = static_cast<int>(width / num_threads);

        // Block scope for async tasks ensure they are waited for, before saving
        // the image.
        {
            std::vector<std::future<void>> tasks;

            for (std::size_t i = 0; i < num_threads; ++i) {
                tasks.push_back(std::async(std::launch::async, [&, i] {
                    render_partial(image, camera, i * stride, (i + 1) * stride);
                }));
            }

            // One last thread in case width is not divisible by num_threads
            if (width % num_threads) {
                tasks.push_back(std::async(std::launch::async, [&] {
                    render_partial(image, camera, num_threads * stride, width);
                }));
            }
        }

        image.saveImage(camera->imageName.c_str());
    }
}

// Parses XML file.
Scene::Scene(const char *xmlPath)
{
    const char *str;
    XMLDocument xmlDoc;
    XMLError eResult;
    XMLElement *pElement;

    maxRecursionDepth = 1;
    shadowRayEps = 0.001;

    eResult = xmlDoc.LoadFile(xmlPath);

    XMLNode *pRoot = xmlDoc.FirstChild();

    pElement = pRoot->FirstChildElement("MaxRecursionDepth");
    if (pElement != nullptr)
        pElement->QueryIntText(&maxRecursionDepth);

    pElement = pRoot->FirstChildElement("BackgroundColor");
    str = pElement->GetText();
    sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g,
           &backgroundColor.b);

    pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
    if (pElement != nullptr)
        pElement->QueryFloatText(&shadowRayEps);

    pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
    if (pElement != nullptr)
        eResult = pElement->QueryFloatText(&intTestEps);

    // Parse cameras
    pElement = pRoot->FirstChildElement("Cameras");
    XMLElement *pCamera = pElement->FirstChildElement("Camera");
    XMLElement *camElement;
    while (pCamera != nullptr) {
        int id;
        char imageName[64];
        vec3f pos, gaze, up;
        ImagePlane imgPlane;

        eResult = pCamera->QueryIntAttribute("id", &id);
        camElement = pCamera->FirstChildElement("Position");
        str = camElement->GetText();
        sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
        camElement = pCamera->FirstChildElement("Gaze");
        str = camElement->GetText();
        sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
        camElement = pCamera->FirstChildElement("Up");
        str = camElement->GetText();
        sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
        camElement = pCamera->FirstChildElement("NearPlane");
        str = camElement->GetText();
        sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right,
               &imgPlane.bottom, &imgPlane.top);
        camElement = pCamera->FirstChildElement("NearDistance");
        eResult = camElement->QueryFloatText(&imgPlane.distance);
        camElement = pCamera->FirstChildElement("ImageResolution");
        str = camElement->GetText();
        sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
        camElement = pCamera->FirstChildElement("ImageName");
        str = camElement->GetText();
        strcpy(imageName, str);

        cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane));

        pCamera = pCamera->NextSiblingElement("Camera");
    }

    // Parse materals
    pElement = pRoot->FirstChildElement("Materials");
    XMLElement *pMaterial = pElement->FirstChildElement("Material");
    XMLElement *materialElement;
    while (pMaterial != nullptr) {
        materials.push_back(new Material());

        int curr = materials.size() - 1;

        eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
        materialElement = pMaterial->FirstChildElement("AmbientReflectance");
        str = materialElement->GetText();
        sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r,
               &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
        materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
        str = materialElement->GetText();
        sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r,
               &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
        materialElement = pMaterial->FirstChildElement("SpecularReflectance");
        str = materialElement->GetText();
        sscanf(str, "%f %f %f", &materials[curr]->specularRef.r,
               &materials[curr]->specularRef.g,
               &materials[curr]->specularRef.b);
        materialElement = pMaterial->FirstChildElement("MirrorReflectance");
        if (materialElement != nullptr) {
            str = materialElement->GetText();
            sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r,
                   &materials[curr]->mirrorRef.g,
                   &materials[curr]->mirrorRef.b);
        } else {
            materials[curr]->mirrorRef.r = 0.0;
            materials[curr]->mirrorRef.g = 0.0;
            materials[curr]->mirrorRef.b = 0.0;
        }
        materialElement = pMaterial->FirstChildElement("PhongExponent");
        if (materialElement != nullptr)
            materialElement->QueryIntText(&materials[curr]->phongExp);

        pMaterial = pMaterial->NextSiblingElement("Material");
    }

    // Parse vertex data
    pElement = pRoot->FirstChildElement("VertexData");
    int cursor = 0;
    vec3f tmpPoint;
    str = pElement->GetText();
    while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
        cursor++;
    while (str[cursor] != '\0') {
        for (int cnt = 0; cnt < 3; cnt++) {
            if (cnt == 0)
                tmpPoint.x = atof(str + cursor);
            else if (cnt == 1)
                tmpPoint.y = atof(str + cursor);
            else
                tmpPoint.z = atof(str + cursor);
            while (str[cursor] != ' ' && str[cursor] != '\t' &&
                   str[cursor] != '\n')
                cursor++;
            while (str[cursor] == ' ' || str[cursor] == '\t' ||
                   str[cursor] == '\n')
                cursor++;
        }
        vertices.push_back(tmpPoint);
    }

    // Parse objects
    pElement = pRoot->FirstChildElement("Objects");

    // Parse spheres
    XMLElement *pObject = pElement->FirstChildElement("Sphere");
    XMLElement *objElement;
    while (pObject != nullptr) {
        int id;
        int matIndex;
        int cIndex;
        float R;

        eResult = pObject->QueryIntAttribute("id", &id);
        objElement = pObject->FirstChildElement("Material");
        eResult = objElement->QueryIntText(&matIndex);
        objElement = pObject->FirstChildElement("Center");
        eResult = objElement->QueryIntText(&cIndex);
        objElement = pObject->FirstChildElement("Radius");
        eResult = objElement->QueryFloatText(&R);

        objects.push_back(new Sphere(id, matIndex, cIndex, R, &vertices));

        pObject = pObject->NextSiblingElement("Sphere");
    }

    // Parse triangles
    pObject = pElement->FirstChildElement("Triangle");
    while (pObject != nullptr) {
        int id;
        int matIndex;
        int p1Index;
        int p2Index;
        int p3Index;

        eResult = pObject->QueryIntAttribute("id", &id);
        objElement = pObject->FirstChildElement("Material");
        eResult = objElement->QueryIntText(&matIndex);
        objElement = pObject->FirstChildElement("Indices");
        str = objElement->GetText();
        sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);

        objects.push_back(
            new Triangle(id, matIndex, p1Index, p2Index, p3Index, &vertices));

        pObject = pObject->NextSiblingElement("Triangle");
    }

    // Parse meshes
    pObject = pElement->FirstChildElement("Mesh");
    while (pObject != nullptr) {
        int id;
        int matIndex;
        int p1Index;
        int p2Index;
        int p3Index;
        int cursor = 0;
        int vertexOffset = 0;
        std::vector<Triangle> faces;
        std::vector<int> *meshIndices = new std::vector<int>;

        eResult = pObject->QueryIntAttribute("id", &id);
        objElement = pObject->FirstChildElement("Material");
        eResult = objElement->QueryIntText(&matIndex);
        objElement = pObject->FirstChildElement("Faces");
        objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
        str = objElement->GetText();
        while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
            cursor++;
        while (str[cursor] != '\0') {
            for (int cnt = 0; cnt < 3; cnt++) {
                if (cnt == 0)
                    p1Index = atoi(str + cursor) + vertexOffset;
                else if (cnt == 1)
                    p2Index = atoi(str + cursor) + vertexOffset;
                else
                    p3Index = atoi(str + cursor) + vertexOffset;
                while (str[cursor] != ' ' && str[cursor] != '\t' &&
                       str[cursor] != '\n')
                    cursor++;
                while (str[cursor] == ' ' || str[cursor] == '\t' ||
                       str[cursor] == '\n')
                    cursor++;
            }
            faces.push_back(*(new Triangle(-1, matIndex, p1Index, p2Index,
                                           p3Index, &vertices)));
            meshIndices->push_back(p1Index);
            meshIndices->push_back(p2Index);
            meshIndices->push_back(p3Index);
        }

        objects.push_back(
            new Mesh(id, matIndex, faces, meshIndices, &vertices));

        pObject = pObject->NextSiblingElement("Mesh");
    }

    // Parse lights
    int id;
    vec3f position;
    vec3f intensity;
    pElement = pRoot->FirstChildElement("Lights");

    XMLElement *pLight = pElement->FirstChildElement("AmbientLight");
    XMLElement *lightElement;
    str = pLight->GetText();
    sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

    pLight = pElement->FirstChildElement("PointLight");
    while (pLight != nullptr) {
        eResult = pLight->QueryIntAttribute("id", &id);
        lightElement = pLight->FirstChildElement("Position");
        str = lightElement->GetText();
        sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
        lightElement = pLight->FirstChildElement("Intensity");
        str = lightElement->GetText();
        sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

        lights.push_back(new PointLight(position, intensity));

        pLight = pLight->NextSiblingElement("PointLight");
    }
}
