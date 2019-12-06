#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Camera.h"
#include "Color.h"
#include "Helpers.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Scene.h"
#include "Translation.h"
#include "Vec3.h"
#include "Vec4.h"
#include "tinyxml2.h"
#include "LineSegment.h"

// Enable this to show ClipTest outputs.
#define CLIPTEST_OUTPUT

using namespace tinyxml2;

extern Scene *scene;

void computeModelingTransform(Model &model)
{
    if (model.compositeTransform) {
        return;
    }

    auto composite_matrix = getIdentityMatrix();
    for (size_t i = 0; i < model.numberOfTransformations; ++i) {
        auto transform_index = model.transformationIds[i] - 1;
        switch (model.transformationTypes[i]) {
            case 'r':
                composite_matrix = multiplyMatrixWithMatrix(
                        scene->rotations[transform_index]->getMatrix(),
                        composite_matrix);
                break;
            case 't':
                composite_matrix = multiplyMatrixWithMatrix(
                        scene->translations[transform_index]->getMatrix(),
                        composite_matrix);
                break;
            case 's':
                composite_matrix = multiplyMatrixWithMatrix(
                        scene->scalings[transform_index]->getMatrix(),
                        composite_matrix);
                break;
        }
    }

    *model.compositeTransform = composite_matrix;
}

void Scene::forwardRenderingPipeline(Camera *camera)
{
    requestsOutsideDrawingArea = 0;  // Clipping test metric
    initializeImage(camera);

    Matrix4 viewingMatrix = camera->getViewingMatrix();

    for (auto &model : scene->models) {
        auto isWireframeMode = model->type == 0;
        model->clippedLineSegments.clear();

        // Compute the composite modeling & viewing transform.
        model->transformedVertices.clear();
        computeModelingTransform(*model);
        Matrix4 transform = multiplyMatrixWithMatrix(viewingMatrix, *model->compositeTransform);

        for (auto &triangle : model->triangles) {
            Vec3 v1_3d = *scene->vertices[triangle.vertexIds[0] - 1];
            Vec3 v2_3d = *scene->vertices[triangle.vertexIds[1] - 1];
            Vec3 v3_3d = *scene->vertices[triangle.vertexIds[2] - 1];
            Vec4 v1 = Vec4(v1_3d.x, v1_3d.y, v1_3d.z, 1., v1_3d.colorId);
            Vec4 v2 = Vec4(v2_3d.x, v2_3d.y, v2_3d.z, 1., v2_3d.colorId);
            Vec4 v3 = Vec4(v3_3d.x, v3_3d.y, v3_3d.z, 1., v3_3d.colorId);
            Vec4 v1_trans = multiplyMatrixWithVec4(transform, v1);
            Vec4 v2_trans = multiplyMatrixWithVec4(transform, v2);
            Vec4 v3_trans = multiplyMatrixWithVec4(transform, v3);

            // Perspective divide
            v1_trans.x /= v1_trans.t;
            v1_trans.y /= v1_trans.t;
            v1_trans.z /= v1_trans.t;
            v1_trans.t = 1.0;
            v2_trans.x /= v2_trans.t;
            v2_trans.y /= v2_trans.t;
            v2_trans.z /= v2_trans.t;
            v2_trans.t = 1.0;
            v3_trans.x /= v3_trans.t;
            v3_trans.y /= v3_trans.t;
            v3_trans.z /= v3_trans.t;
            v3_trans.t = 1.0;

            if (!cullingEnabled) {
                model->transformedVertices.push_back(v1_trans);
                model->transformedVertices.push_back(v2_trans);
                model->transformedVertices.push_back(v3_trans);
            } else {
                Vec3 centerOfMass = {
                    (v1_trans.x + v2_trans.x + v3_trans.x) / 3.0,
                    (v2_trans.y + v2_trans.y + v3_trans.y) / 3.0,
                    (v2_trans.z + v2_trans.z + v3_trans.z) / 3.0,
                    0
                };
                Vec3 normal = crossProductVec3(
                    {v3_trans.x - v1_trans.x,
                     v3_trans.y - v1_trans.y,
                     v3_trans.z - v1_trans.z,
                     0},
                    {v2_trans.x - v1_trans.x,
                     v2_trans.y - v1_trans.y,
                     v2_trans.z - v1_trans.z,
                     0}
                );

                // Here camera coordinates are 0, 0, 0 --> so centerOfMass is the camera-to-object vector.
                auto test = dotProductVec3(centerOfMass, normal);

                if (test < 0) {
                    // Front-facing face
                    model->transformedVertices.push_back(v1_trans);
                    model->transformedVertices.push_back(v2_trans);
                    model->transformedVertices.push_back(v3_trans);
                } // else: Back-facing face, must be culled.
            }
        }

        if (isWireframeMode && model->clippedLineSegments.empty()) {
            // Prepare line segments for culling.
            for (size_t i = 0; i <= model->transformedVertices.size() - 3; i += 3) {
                auto v1 = model->transformedVertices[i+0];
                auto v2 = model->transformedVertices[i+1];
                auto v3 = model->transformedVertices[i+2];

                LineSegment ls_v1_v2(v1, v2);
                LineSegment ls_v2_v3(v2, v3);
                LineSegment ls_v3_v1(v3, v1);

                ls_v1_v2.clip();
                ls_v2_v3.clip();
                ls_v3_v1.clip();

                model->clippedLineSegments.push_back(ls_v1_v2);
                model->clippedLineSegments.push_back(ls_v2_v3);
                model->clippedLineSegments.push_back(ls_v3_v1);
            }
        }

        auto viewport = camera->getViewportMatrix();

        if (isWireframeMode) {
            for (const auto& ls : model->clippedLineSegments) {
                if (!ls.visible) continue;

                auto ls_from_view = multiplyMatrixWithVec4(viewport, ls.a);
                auto ls_to_view = multiplyMatrixWithVec4(viewport, ls.b);

                Vec3 ls_from_view_3d{ls_from_view.x, ls_from_view.y, ls_from_view.z, ls_from_view.colorId};
                Vec3 ls_to_view_3d{ls_to_view.x, ls_to_view.y, ls_to_view.z, ls_to_view.colorId};

                drawLine(ls_from_view_3d, ls_to_view_3d);
            }
        } else { // Solid mode
            for (size_t i = 0; i <= model->transformedVertices.size() - 3; i += 3) {
                auto v1_view = multiplyMatrixWithVec4(viewport, model->transformedVertices[i + 0]);
                auto v2_view = multiplyMatrixWithVec4(viewport, model->transformedVertices[i + 1]);
                auto v3_view = multiplyMatrixWithVec4(viewport, model->transformedVertices[i + 2]);

                Vec3 a = {v1_view.x, v1_view.y, v1_view.z, v1_view.colorId},
                     b = {v2_view.x, v2_view.y, v2_view.z, v2_view.colorId},
                     c = {v3_view.x, v3_view.y, v3_view.z, v3_view.colorId};

                drawTriangle(a, b, c);
            }
        }
    }

    writeImageToPPMFile(camera);
    convertPPMToPNG(camera->outputFileName, /* osType = */ 1);

#ifdef CLIPTEST_OUTPUT
    std::cerr << "[ClipTest] Requests outside drawing area for " << camera->outputFileName
              << ": " << requestsOutsideDrawingArea << "\n";
#endif
    requestsOutsideDrawingArea = 0;
}

Scene::Scene(const char *xmlPath)
{
    const char *str;
    XMLDocument xmlDoc;
    XMLElement *pElement;

    xmlDoc.LoadFile(xmlPath);

    XMLNode *pRoot = xmlDoc.FirstChild();

    // read background color
    pElement = pRoot->FirstChildElement("BackgroundColor");
    str = pElement->GetText();
    sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g,
           &backgroundColor.b);

    // read culling
    pElement = pRoot->FirstChildElement("Culling");
    if (pElement != nullptr)
        pElement->QueryBoolText(&cullingEnabled);

    // read projection type
    pElement = pRoot->FirstChildElement("ProjectionType");
    if (pElement != nullptr)
        pElement->QueryIntText(&projectionType);

    // read cameras
    pElement = pRoot->FirstChildElement("Cameras");
    XMLElement *pCamera = pElement->FirstChildElement("Camera");
    XMLElement *camElement;
    while (pCamera != nullptr) {
        auto *cam = new Camera();

        pCamera->QueryIntAttribute("id", &cam->cameraId);

        camElement = pCamera->FirstChildElement("Position");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

        camElement = pCamera->FirstChildElement("Gaze");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);

        camElement = pCamera->FirstChildElement("Up");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

        cam->gaze = normalizeVec3(cam->gaze);
        cam->u = crossProductVec3(cam->gaze, cam->v);
        cam->u = normalizeVec3(cam->u);

        cam->w = inverseVec3(cam->gaze);
        cam->v = crossProductVec3(cam->u, cam->gaze);
        cam->v = normalizeVec3(cam->v);

        camElement = pCamera->FirstChildElement("ImagePlane");
        str = camElement->GetText();
        sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d", &cam->left, &cam->right,
               &cam->bottom, &cam->top, &cam->near, &cam->far, &cam->horRes,
               &cam->verRes);

        camElement = pCamera->FirstChildElement("OutputName");
        str = camElement->GetText();
        cam->outputFileName = std::string(str);

        cameras.push_back(cam);

        pCamera = pCamera->NextSiblingElement("Camera");
    }

    // read vertices
    pElement = pRoot->FirstChildElement("Vertices");
    XMLElement *pVertex = pElement->FirstChildElement("Vertex");
    int vertexId = 1;

    while (pVertex != nullptr) {
        Vec3 *vertex = new Vec3();
        auto *color = new Color();

        vertex->colorId = vertexId;

        str = pVertex->Attribute("position");
        sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

        str = pVertex->Attribute("color");
        sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

        vertices.push_back(vertex);
        colorsOfVertices.push_back(color);

        pVertex = pVertex->NextSiblingElement("Vertex");

        vertexId++;
    }

    // read translations
    pElement = pRoot->FirstChildElement("Translations");
    XMLElement *pTranslation = pElement->FirstChildElement("Translation");
    while (pTranslation != nullptr) {
        auto *translation = new Translation();

        pTranslation->QueryIntAttribute("id", &translation->translationId);

        str = pTranslation->Attribute("value");
        sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty,
               &translation->tz);

        translations.push_back(translation);

        pTranslation = pTranslation->NextSiblingElement("Translation");
    }

    // read scalings
    pElement = pRoot->FirstChildElement("Scalings");
    XMLElement *pScaling = pElement->FirstChildElement("Scaling");
    while (pScaling != nullptr) {
        auto *scaling = new Scaling();

        pScaling->QueryIntAttribute("id", &scaling->scalingId);
        str = pScaling->Attribute("value");
        sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

        scalings.push_back(scaling);

        pScaling = pScaling->NextSiblingElement("Scaling");
    }

    // read rotations
    pElement = pRoot->FirstChildElement("Rotations");
    XMLElement *pRotation = pElement->FirstChildElement("Rotation");
    while (pRotation != nullptr) {
        auto *rotation = new Rotation();

        // GIRAFFE BEGIN
        double angle, ux, uy, uz;
        pRotation->QueryIntAttribute("id", &rotation->rotationId);
        str = pRotation->Attribute("value");
        sscanf(str, "%lf %lf %lf %lf", &angle, &ux, &uy, &uz);
        rotation->angle = angle;
        rotation->u = Vec3(ux, uy, uz, /* colorId = */ 0);
        // GIRAFFE END

        rotations.push_back(rotation);

        pRotation = pRotation->NextSiblingElement("Rotation");
    }

    // read models
    pElement = pRoot->FirstChildElement("Models");

    XMLElement *pModel = pElement->FirstChildElement("Model");
    XMLElement *modelElement;
    while (pModel != nullptr) {
        auto *model = new Model();

        pModel->QueryIntAttribute("id", &model->modelId);
        pModel->QueryIntAttribute("type", &model->type);

        // read model transformations
        XMLElement *pTransformations =
            pModel->FirstChildElement("Transformations");
        XMLElement *pTransformation =
            pTransformations->FirstChildElement("Transformation");

        pTransformations->QueryIntAttribute("count",
                                            &model->numberOfTransformations);

        while (pTransformation != nullptr) {
            char transformationType;
            int transformationId;

            str = pTransformation->GetText();
            sscanf(str, "%c %d", &transformationType, &transformationId);

            model->transformationTypes.push_back(transformationType);
            model->transformationIds.push_back(transformationId);

            pTransformation =
                pTransformation->NextSiblingElement("Transformation");
        }

        // read model triangles
        XMLElement *pTriangles = pModel->FirstChildElement("Triangles");
        XMLElement *pTriangle = pTriangles->FirstChildElement("Triangle");

        pTriangles->QueryIntAttribute("count", &model->numberOfTriangles);

        while (pTriangle != nullptr) {
            int v1, v2, v3;

            str = pTriangle->GetText();
            sscanf(str, "%d %d %d", &v1, &v2, &v3);

            model->triangles.emplace_back(v1, v2, v3);

            pTriangle = pTriangle->NextSiblingElement("Triangle");
        }

        models.push_back(model);

        pModel = pModel->NextSiblingElement("Model");
    }
}

void Scene::initializeImage(Camera *camera)
{
    if (this->image.empty()) {
        for (int i = 0; i < camera->horRes; i++) {
            std::vector<Color> rowOfColors;

            rowOfColors.reserve(camera->verRes);
            for (int j = 0; j < camera->verRes; j++) {
                rowOfColors.push_back(this->backgroundColor);
            }

            this->image.push_back(rowOfColors);
        }
    }
    // if image is filled before, just change color rgb values with the
    // background color
    else {
        for (int i = 0; i < camera->horRes; i++) {
            for (int j = 0; j < camera->verRes; j++) {
                this->image[i][j].r = this->backgroundColor.r;
                this->image[i][j].g = this->backgroundColor.g;
                this->image[i][j].b = this->backgroundColor.b;
            }
        }
    }
}

int Scene::makeBetweenZeroAnd255(double value)
{
    if (value >= 255.0)
        return 255;
    if (value <= 0.0)
        return 0;
    return (int)(value);
}

void Scene::writeImageToPPMFile(Camera *camera)
{
    std::ofstream fout;

    fout.open(camera->outputFileName.c_str());

    fout << "P3" << std::endl;
    fout << "# " << camera->outputFileName << std::endl;
    fout << camera->horRes << " " << camera->verRes << std::endl;
    fout << "255" << std::endl;

    for (int j = camera->verRes - 1; j >= 0; j--) {
        for (int i = 0; i < camera->horRes; i++) {
            fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
                 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
                 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
        }
        fout << std::endl;
    }
    fout.close();
}

void Scene::convertPPMToPNG(const std::string& ppmFileName, int osType)
{
    std::string command;

    // call command on Ubuntu
    if (osType == 1) {
        command = "convert " + ppmFileName + " " + ppmFileName + ".png";
        system(command.c_str());
    }

    // call command on Windows
    else if (osType == 2) {
        command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
        system(command.c_str());
    }

    // default action - don't do conversion
    else {}
}

double fab(int x_a, int y_a, int x_b, int y_b, int x, int y)
{
   return (y_a - y_b) * x + (x_b - x_a) * y + (x_a * y_b) - (x_b * y_a);
}

void Scene::drawPixel(int i, int j, const Color& c) {
    if (i < 0 || j < 0 || i >= image.size() || j >= image[0].size())
    {
        requestsOutsideDrawingArea++;
        return;
    }

    image[i][j] = c;
}

void Scene::drawTriangle(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
    int
        x_a = static_cast<int>(a.x),
        x_b = static_cast<int>(b.x),
        x_c = static_cast<int>(c.x),
        x_min = std::min(std::min(x_a, x_b), x_c),
        x_max = std::max(std::max(x_a, x_b), x_c),
        y_a = static_cast<int>(a.y),
        y_b = static_cast<int>(b.y),
        y_c = static_cast<int>(c.y),
        y_min = std::min(std::min(y_a, y_b), y_c),
        y_max = std::max(std::max(y_a, y_b), y_c)
    ;

    double
        alpha,
        beta,
        gamma,
        f_alpha = fab(x_a, y_a, x_b, y_b, x_c, y_c),
        f_beta = fab(x_c, y_c, x_a, y_a, x_b, y_b),
        f_gamma = fab(x_a, y_a, x_b, y_b, x_c, y_c)
    ;

    Color color = {0, 0, 0};

    for (int y = y_min; y < y_max; ++y) {
        for (int x = x_min; x < x_max; ++x) {
            double
                f_bc_xy = fab(x_b, y_b, x_c, y_c, x, y),
                f_ca_xy = fab(x_c, y_c, x_a, y_a, x, y),
                f_ab_xy = fab(x_a, y_a, x_b, y_b, x, y)
            ;

            alpha = f_bc_xy / f_alpha;
            beta = f_ca_xy / f_beta;
            gamma = f_ab_xy / f_gamma;

            if (alpha >= 0 && beta >= 0 && gamma >= 0) {
                Color color_a = getColorById(a.colorId),
                      color_b = getColorById(b.colorId),
                      color_c = getColorById(c.colorId);

                color = {
                    (alpha * color_a.r + beta * color_b.r + gamma * color_c.r),
                    (alpha * color_a.g + beta * color_b.g + gamma * color_c.g),
                    (alpha * color_a.b + beta * color_b.b + gamma * color_c.b)
                };

                drawPixel(x, y, color);
            }
        }
    }
}

void Scene::drawLine(const Vec3& from, const Vec3& to) {
    int x_from = static_cast<int>(from.x),
        y_from = static_cast<int>(from.y),
        x_to = static_cast<int>(to.x),
        y_to = static_cast<int>(to.y);

    if (x_from == x_to) {
        return drawLineVertical(from, to);
    } else if (y_from == y_to) {
        return drawLineHorizontal(from, to);
    } else {
        auto slope = static_cast<double>(y_to - y_from) / (x_to - x_from);

        if (slope < -1) {
            return drawLineQuad7(from, to);
        } else if (slope < 0) {
            return drawLineQuad8(from, to);
        } else if (slope < 1) {
            return drawLineQuad1(from, to);
        } else {
            return drawLineQuad2(from, to);
        }
    }
}

void Scene::drawLineVertical(const Vec3& from, const Vec3& to) {
    int y_from = static_cast<int>(from.y),
        y_to = static_cast<int>(to.y),
        x = static_cast<int>(from.x);

    if (y_from > y_to)
        return drawLineVertical(to, from);

    Color color_from, color_to, color_current, color_diff;
    color_from = getColorById(from.colorId);
    color_current = color_from;
    color_to = getColorById(to.colorId);
    color_diff = {
            (color_to.r - color_from.r) / (y_to - y_from),
            (color_to.g - color_from.g) / (y_to - y_from),
            (color_to.b - color_from.b) / (y_to - y_from)
    };

    for (int y_current = y_from; y_current < y_to; ++y_current) {
        drawPixel(x, y_current, color_current);

        color_current.r += color_diff.r;
        color_current.g += color_diff.g;
        color_current.b += color_diff.b;
    }
}

void Scene::drawLineHorizontal(const Vec3& from, const Vec3& to) {
    int x_from = static_cast<int>(from.x),
        x_to = static_cast<int>(to.x),
        y = static_cast<int>(from.y);

    if (x_from > x_to)
        return drawLineHorizontal(to, from);

    Color color_from, color_to, color_current, color_diff;
    color_from = getColorById(from.colorId);
    color_current = color_from;
    color_to = getColorById(to.colorId);
    color_diff = {
            (color_to.r - color_from.r) / (x_to - x_from),
            (color_to.g - color_from.g) / (x_to - x_from),
            (color_to.b - color_from.b) / (x_to - x_from)
    };

    for (int x_current = x_from; x_current < x_to; ++x_current) {
        drawPixel(x_current, y, color_current);

        color_current.r += color_diff.r;
        color_current.g += color_diff.g;
        color_current.b += color_diff.b;
    }
}

void Scene::drawLineQuad1(const Vec3& from, const Vec3& to) {
    int x_from = static_cast<int>(from.x),
        y_from = static_cast<int>(from.y),
        x_to = static_cast<int>(to.x),
        y_to = static_cast<int>(to.y);

    if (x_from > x_to)
        return drawLineQuad1(to, from);

    Color color_from, color_to, color_current, color_diff;
    color_from = getColorById(from.colorId);
    color_current = color_from;
    color_to = getColorById(to.colorId);
    color_diff = {
        (color_to.r - color_from.r) / (x_to - x_from),
        (color_to.g - color_from.g) / (x_to - x_from),
        (color_to.b - color_from.b) / (x_to - x_from)
    };

    int y_current = y_from;
    double test = (y_from - y_to) + 0.5 * (x_to - x_from);

    for (int x_current = x_from; x_current < x_to; ++x_current) {
        drawPixel(x_current, y_current, color_current);

        if (test < 0) {
            y_current += 1;
            test += (y_from - y_to) + (x_to - x_from);
        } else {
            test += (y_from - y_to);
        }

        color_current.r += color_diff.r;
        color_current.g += color_diff.g;
        color_current.b += color_diff.b;
    }
}

void Scene::drawLineQuad2(const Vec3& from, const Vec3& to) {
    int x_from = static_cast<int>(from.x),
            y_from = static_cast<int>(from.y),
            x_to = static_cast<int>(to.x),
            y_to = static_cast<int>(to.y);

    if (x_from > x_to)
        return drawLineQuad2(to, from);

    Color color_from, color_to, color_current, color_diff;
    color_from = getColorById(from.colorId);
    color_current = color_from;
    color_to = getColorById(to.colorId);
    color_diff = {
            (color_to.r - color_from.r) / (y_to - y_from),
            (color_to.g - color_from.g) / (y_to - y_from),
            (color_to.b - color_from.b) / (y_to - y_from)
    };

    int x_current = x_from;
    double test = (x_from - x_to) + 0.5 * (y_to - y_from);

    for (int y_current = y_from; y_current < y_to; ++y_current) {
        drawPixel(x_current, y_current, color_current);

        if (test < 0) {
            x_current += 1;
            test += (x_from - x_to) + (y_to - y_from);
        } else {
            test += (x_from - x_to);
        }

        color_current.r += color_diff.r;
        color_current.g += color_diff.g;
        color_current.b += color_diff.b;
    }
}

void Scene::drawLineQuad7(const Vec3& from, const Vec3& to) {
    int x_from = static_cast<int>(from.x),
        y_from = static_cast<int>(from.y),
        x_to = static_cast<int>(to.x),
        y_to = static_cast<int>(to.y);

    if (x_from > x_to)
        return drawLineQuad7(to, from);

    Color color_from, color_to, color_current, color_diff;
    color_from = getColorById(from.colorId);
    color_current = color_from;
    color_to = getColorById(to.colorId);
    color_diff = {
            -(color_to.r - color_from.r) / (y_to - y_from),
            -(color_to.g - color_from.g) / (y_to - y_from),
            -(color_to.b - color_from.b) / (y_to - y_from)
    };

    int x_current = x_from;
    double test = (x_from - x_to) + 0.5 * (y_from - y_to);

    for (int y_current = y_from; y_current > y_to; --y_current) {
        drawPixel(x_current, y_current, color_current);

        if (test < 0) {
            x_current += 1;
            test += (x_from - x_to) + (y_from - y_to);
        } else {
            test += (x_from - x_to);
        }

        color_current.r += color_diff.r;
        color_current.g += color_diff.g;
        color_current.b += color_diff.b;
    }
}

void Scene::drawLineQuad8(const Vec3& from, const Vec3& to) {
    int x_from = static_cast<int>(from.x),
        y_from = static_cast<int>(from.y),
        x_to = static_cast<int>(to.x),
        y_to = static_cast<int>(to.y);

    if (x_from > x_to)
        return drawLineQuad8(to, from);

    Color color_from, color_to, color_current, color_diff;
    color_from = getColorById(from.colorId);
    color_current = color_from;
    color_to = getColorById(to.colorId);
    color_diff = {
            (color_to.r - color_from.r) / (x_to - x_from),
            (color_to.g - color_from.g) / (x_to - x_from),
            (color_to.b - color_from.b) / (x_to - x_from)
    };

    int y_current = y_from;
    double test = (y_to - y_from) + 0.5 * (x_to - x_from);

    for (int x_current = x_from; x_current < x_to; ++x_current) {
        drawPixel(x_current, y_current, color_current);

        if (test < 0) {
            y_current -= 1;
            test += (y_to - y_from) + (x_to - x_from);
        } else {
            test += (y_to - y_from);
        }

        color_current.r += color_diff.r;
        color_current.g += color_diff.g;
        color_current.b += color_diff.b;
    }
}

Color Scene::getColorById(int colorId) {
    if (colorId > 0) {
        return *colorsOfVertices[colorId - 1];
    } else if (colorId < 0) {
        return customColors[-colorId - 1];
    }

    assert(false);
}
