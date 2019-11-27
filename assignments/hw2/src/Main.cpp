#include "Helpers.h"
#include "Matrix4.h"
#include "Scene.h"
#include <iostream>
#include <string>
#include <vector>

Scene *scene;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Please run the rasterizer as:" << std::endl
                  << "\t./rasterizer <input_file_name>" << std::endl;
        return 1;
    } else {
        const char *xmlPath = argv[1];

        scene = new Scene(xmlPath);

        for (int i = 0; i < scene->cameras.size(); i++) {
            // initialize image with basic values
            scene->initializeImage(scene->cameras[i]);

            // do forward rendering pipeline operations
            scene->forwardRenderingPipeline(scene->cameras[i]);

            // generate PPM file
            scene->writeImageToPPMFile(scene->cameras[i]);

            // Converts PPM image in given path to PNG file, by calling
            // ImageMagick's 'convert' command. Notice that os_type is not given
            // as 1 (Ubuntu) or 2 (Windows), below call doesn't do conversion.
            // Change os_type to 1 or 2, after being sure that you have
            // ImageMagick installed.
            scene->convertPPMToPNG(scene->cameras[i]->outputFileName, 99);
        }

        return 0;
    }
}
