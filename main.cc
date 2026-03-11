#include <iostream>
#include <fstream>

#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "polygon.h"

#include <memory>
#include <vector>


// Global variables

auto CameraLookAt = point3(0, 0, 0);
auto CameraLookFrom = point3(0, 0, 1);
auto CameraLookUp = point3(0, 1, 0);
auto FieldOfView = 90.0;

color BackgroundColor = color(0.2, 0.2, 0.2);

const double infinity = std::numeric_limits<double>::infinity();

std::vector<std::shared_ptr<sphere>> spheres;
//std::vector<sphere> spheres;
std::vector<polygon> polygons;


color get_illumination(const ray& r) {

    std::vector<sphere> spheres;

    // sphere(center, radius, Kd, Ks, Ka, Kgls, Refl, Od, Os)
    // reflective sphere 
    spheres.push_back(
        sphere(
            point3(0.0, 0.3, -1.0), 
            0.25, 0.0, 0.1, 0.1, 10.0, 0.125, 
            color(0.75, 0.75, 0.75), 
color(1.0, 1.0, 1.0)
        )
    );

    // polygon(a, b, c, Kd, Ks, Ka, Kgls, Refl, Od, Os)
	// blue triangle
    polygons.push_back(
        polygon(
            point3(0.0, -0.7, -0.5),
            point3(1.0, 0.4, -1.8),
            point3(0.0, -0.7, -1.5),
            0.9, 1.0, 0.1, 4.0, 0.0,
            color(0.0, 0.0, 1.0),
            color(1.0, 1.0, 1.0)
        )
    );
    // yellow triangle
    polygons.push_back(
        polygon(
            point3(0.0, -0.7, -0.5),
            point3(0.0, -0.7, -1.5),
            point3(-1.0, 0.4, -1.0),
            0.9, 1.0, 0.1, 4.0, 0.0, 
            color(1.0, 0.0, 0.0),
            color(1.0, 1.0, 1.0)
        )
    );

    color finalColor = BackgroundColor;
    double closest_t = infinity;

    for (auto& s : spheres) {
        auto t = s.hit(r);

        if (t > 0.0 && t < closest_t) {
            closest_t = t;
            auto t = s.hit(r);
            finalColor = s.get_color(r, t, 3, spheres);
        }
    }
    for (auto& p : polygons) {
        auto t = p.hit(r);

        if (t > 0.0 && t < closest_t) {
            closest_t = t;
            auto t = p.hit(r);
            finalColor = p.get_color(r, t, 3);
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

	auto aspect_ratio = 8.0 / 8.0;
    int image_width = 200;
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