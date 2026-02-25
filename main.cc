#include <iostream>
#include <fstream>

#include "color.h"
#include "vec3.h"

int main() {

	// Create Output File

	std::ofstream imageOut;
	imageOut.open("../../image.ppm");

    if (!imageOut.is_open()) {
		std::cerr << "File not open." << std::endl;
        return 1;
	}

    // Create Image

    int image_width = 256;
    int image_height = 256;

    // Render Image

    imageOut << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto pixel_color = color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0);
            write_color(imageOut, pixel_color);
           
            // light pink color

            //int ir = int(255);
            //int ig = int(192);
            //int ib = int(203);

            //vec3 value;
			// imageOut << value.x << ' ' << value.y << ' ' << value.z << '\n';
            //std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    std::clog << "\rDone.                 \n";
	imageOut.close();
}

// terminal run command
// cmake -B build | cmake --build build | build\Debug\RayTracer.exe > image.ppm
// C:\Users\Sam\OneDrive\Desktop\Grad School\ComputerGraphics\Ray Tracer Files\images\image.ppm