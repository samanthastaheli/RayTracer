#include <iostream>
#include <fstream>

#include "color.h"
#include "vec3.h"
#include "ray.h"


// Functions to create colors and shapes

bool hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = center - r.origin();
    auto a = dot(r.direction(), r.direction());
    auto b = -2.0 * dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    return (discriminant >= 0);
}


color ray_color(const ray& r) {
    if (hit_sphere(point3(0, 0, -1), 0.5, r))
        //return color(255, 192, 203); // light pink color
        return color(1, 0, 0);
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
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

	auto aspect_ratio = 16.0 / 9.0; // aspect ratio choosen from ray tracing in one weekend
    int image_width = 400;
    int image_height = int(image_width / aspect_ratio);
    // ensure height is at least 1
	image_height = (image_height < 1) ? 1 : image_height;

    // Create camera and viewport

    auto focal_length = 1.0;
	auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(image_width) / image_height);
    auto camera_center = point3(0, 0, 0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel
    auto viewport_upper_left = camera_center
        - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Render Image

    imageOut << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);            
            write_color(imageOut, pixel_color);
           
            

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