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


color get_illumination(const ray& r) {

    std::vector<sphere> spheres;

    // sphere(center, radius, Kd, Ks, Ka, Kgls, Od, Os)
    // Ground (big green sphere below)
    spheres.push_back(
        sphere(point3(0.0, -1000.5, 0.0), 1000.0, 0.9, 0.0, 0.1, 16.0, color(0.576, 0.686, 0.353), color(1.0, 1.0, 1.0))
    );

    // Sky (big blue sphere above)
    spheres.push_back(
        sphere(point3(0.0, 1500.5, 0.0), 1000.0, 0.9, 0.0, 0.1, 16.0, color(0.741, 0.878, 0.996), color(1.0, 1.0, 1.0))
    );

    // Three spheres in a row
    spheres.push_back(
        sphere(point3(-0.6, 0.0, -1.0), 0.2, 0.8, 0.8, 0.5, 64.0, color(0.804, 0.706, 0.859), color(1.0, 1.0, 1.0))
    );
    spheres.push_back(
        sphere(point3(0.0, 0.0, -1.0), 0.2, 0.8, 0.8, 0.5, 64.0, color(1.0, 0.784, 0.867), color(1.0, 1.0, 1.0))
    );
    spheres.push_back(
        sphere(point3(0.6, 0.0, -1.0), 0.2, 0.8, 0.8, 0.5, 64.0, color(1.0, 0.686, 0.8), color(1.0, 1.0, 1.0))
    );

    color finalColor = BackgroundColor;
    double closest_t = infinity;

    for (auto& s : spheres) {
        auto t = s.hit(r);

        if (t > 0.0 && t < closest_t) {
            closest_t = t;
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