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
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"
#include "tinyxml2.h"

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
    initializeImage(camera);

    Matrix4 viewingMatrix = camera->getViewingMatrix();

    for (auto &model : scene->models) {
        model->transformedVertices.clear();
        computeModelingTransform(*model);

        Matrix4 transform = multiplyMatrixWithMatrix(
            viewingMatrix,
            *model->compositeTransform
        );

        for (auto &triangle : model->triangles) {
            Vec3 vertex1_v3 = *scene->vertices[triangle.vertexIds[0] - 1];
            Vec3 vertex2_v3 = *scene->vertices[triangle.vertexIds[1] - 1];
            Vec3 vertex3_v3 = *scene->vertices[triangle.vertexIds[2] - 1];

            Vec4 vertex1 = Vec4(vertex1_v3.x,
                                vertex1_v3.y,
                                vertex1_v3.z,
                                1., 0.);
            Vec4 vertex2 = Vec4(vertex2_v3.x,
                                vertex2_v3.y,
                                vertex2_v3.z,
                                1., 0.);
            Vec4 vertex3 = Vec4(vertex3_v3.x,
                                vertex3_v3.y,
                                vertex3_v3.z,
                                1., 0.);

            Vec4 vertex1_transformed = multiplyMatrixWithVec4(transform, vertex1);
            Vec4 vertex2_transformed = multiplyMatrixWithVec4(transform, vertex2);
            Vec4 vertex3_transformed = multiplyMatrixWithVec4(transform, vertex3);

            model->transformedVertices.push_back(vertex1_transformed);
            model->transformedVertices.push_back(vertex2_transformed);
            model->transformedVertices.push_back(vertex3_transformed);
        }
    }

    for (auto &model : scene->models) {
        auto &vertices = model->transformedVertices;
        for (size_t i = 0; i < vertices.size() - 3; i += 3) {
            auto &a = vertices[i];
            auto &b = vertices[i + 1];
            auto &c = vertices[i + 2];

            {
                auto t_e = 0.0, t_l = 1.0;
                auto visible = false;
                auto d_x = b.x - a.x;
                auto d_y = b.y - a.y;
                auto d_z = b.z - a.z;

                if (line_visible(d_x, -1 - a.x, t_e, t_l)) {
                if (line_visible(-d_x, a.x - 1, t_e, t_l)) {
                if (line_visible(d_y, -1 - a.y, t_e, t_l)) {
                if (line_visible(-d_y, a.y - 1, t_e, t_l)) {
                if (line_visible(d_z, -1 - a.z, t_e, t_l)) {
                if (line_visible(-d_z, a.z - 1, t_e, t_l)) {
                    visible = true;
                    if (t_l < 1) {
                        b.x = a.x + d_x*t_l;
                        b.y = a.y + d_y*t_l;
                        b.z = a.z + d_z*t_l;
                    }
                    if (t_e) {
                        a.x = b.x + d_x*t_e;
                        a.y = b.y + d_y*t_e;
                        a.z = b.z + d_z*t_e;
                    }
                }}}}}}
            }
        }
    }

    writeImageToPPMFile(camera);
    convertPPMToPNG(camera->outputFileName, /* osType = */ 1);
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
    if (pElement != NULL)
        pElement->QueryBoolText(&cullingEnabled);

    // read projection type
    pElement = pRoot->FirstChildElement("ProjectionType");
    if (pElement != NULL)
        pElement->QueryIntText(&projectionType);

    // read cameras
    pElement = pRoot->FirstChildElement("Cameras");
    XMLElement *pCamera = pElement->FirstChildElement("Camera");
    XMLElement *camElement;
    while (pCamera != NULL) {
        Camera *cam = new Camera();

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

    while (pVertex != NULL) {
        Vec3 *vertex = new Vec3();
        Color *color = new Color();

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
    while (pTranslation != NULL) {
        Translation *translation = new Translation();

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
    while (pScaling != NULL) {
        Scaling *scaling = new Scaling();

        pScaling->QueryIntAttribute("id", &scaling->scalingId);
        str = pScaling->Attribute("value");
        sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

        scalings.push_back(scaling);

        pScaling = pScaling->NextSiblingElement("Scaling");
    }

    // read rotations
    pElement = pRoot->FirstChildElement("Rotations");
    XMLElement *pRotation = pElement->FirstChildElement("Rotation");
    while (pRotation != NULL) {
        Rotation *rotation = new Rotation();

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
    while (pModel != NULL) {
        Model *model = new Model();

        pModel->QueryIntAttribute("id", &model->modelId);
        pModel->QueryIntAttribute("type", &model->type);

        // read model transformations
        XMLElement *pTransformations =
            pModel->FirstChildElement("Transformations");
        XMLElement *pTransformation =
            pTransformations->FirstChildElement("Transformation");

        pTransformations->QueryIntAttribute("count",
                                            &model->numberOfTransformations);

        while (pTransformation != NULL) {
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

        while (pTriangle != NULL) {
            int v1, v2, v3;

            str = pTriangle->GetText();
            sscanf(str, "%d %d %d", &v1, &v2, &v3);

            model->triangles.push_back(Triangle(v1, v2, v3));

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

void Scene::convertPPMToPNG(std::string ppmFileName, int osType)
{
    std::string command;

    // call command on Ubuntu. OK boss
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
