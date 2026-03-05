#include <iostream>
#include <fstream>

#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "sphere.h"

#include <memory>
#include <vector>


// Global variables

auto CameraLookAt = point3(0, 0, 0);
auto CameraLookFrom = point3(0, 0, 1);
auto CameraLookUp = point3(0, 1, 0);
auto FieldOfView = 90.0;

const double infinity = std::numeric_limits<double>::infinity();

std::vector<std::shared_ptr<sphere>> spheres;


// Functions to create colors and shapes


color get_illumination(const ray& r) {
    /*
    slide 12 and 13 in IlluminationShading
    Equations for I and R:
        I = ambientTerm + diffuseTerm + specularTerm
        ambientTerm = k_a * Ia * O_d
        diffuseTerm = k_d * I_p * O_d * max(0, N
        specularTerm = k_s * I_p * O_s * max(0, R dot O_s)^n

        R = reflect direction 2N(N dot L) - L
    */

    //auto sphere1 = sphere(point3(0, 0, -1), 0.5);

    std::vector<sphere> spheres;
    // sphere(center, radius, Kd, Ks, Ka, Kgls, Od, Os)
    spheres.push_back(sphere(point3(0.0, -10000.5, 0.0), 10000.0, 0.9, 0.0, 0.1, 16.0, color(0.0, 0.0, 1.0), color(1.0, 1.0, 1.0))); // blue sphere
    spheres.push_back(sphere(point3(-0.6, 0.0, 0.0), 0.3, 0.7, 0.2, 0.1, 64.0, color(0.0, 1.0, 0.0), color(0.5, 1.0, 0.5))); // green sphere
    spheres.push_back(sphere(point3(0.45, 0.0, -0.15), 0.15, 0.8, 0.1, 0.3, 4.0, color(1.0, 1.0, 1.0), color(1.0, 1.0, 1.0))); // white sphere
    spheres.push_back(sphere(point3(0, 0, -1), 0.2, 0.6, 0.3, 0.1, 32.0, color(1.0, 0.0, 0.0), color(1.0, 1.0, 1.0))); // red sphere

    color finalColor = BackgroundColor;

    for (auto& s : spheres) {
        auto t = s.hit(r);

        if (t > 0.0) {
            finalColor = s.get_color(r);
        }
    }
    return finalColor;
}


int main() {

	// Create Output File

	std::ofstream imageOut;
	imageOut.open("../../image.ppm");

    if (!imageOut.is_open()) {
		std::cerr << "File not open." << std::endl;
        return 1;
	}

    // Create Image

	//auto aspect_ratio = 16.0 / 9.0; // aspect ratio choosen from ray tracing in one weekend
    auto aspect_ratio = 16.0 / 16.0;
    int image_width = 400;
    int image_height = int(image_width / aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height; // ensure height is at least 1

    // Create camera and viewport

    auto focal_length = 1.0;
	auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);

    // Calculate the vectors across the horizontal and down the vertical viewport edges
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel
    auto viewport_upper_left = CameraLookAt - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render Image

    imageOut << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - CameraLookAt;
            ray r(CameraLookAt, ray_direction);

            color pixel_color = get_illumination(r);            
            write_color(imageOut, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
	imageOut.close();
}

// terminal build command
// cmake -B build | cmake --build build