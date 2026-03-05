#include <iostream>
#include <fstream>

#include "color.h"
#include "vec3.h"
#include "ray.h"

// Global variables

auto CameraLookAt = point3(0, 0, 0);
auto CameraLookFrom = point3(0, 0, 1);
auto CameraLookUp = point3(0, 1, 0);
auto FieldOfView = 90.0;


color BackgroundColor = color(0.2, 0.2, 0.2);


// Functions to create colors and shapes

double hit_sphere(const point3& center, double radius, const ray& r) {
    // h = d * (C - Q)
    // chnage to t0 and t1 for two solutions to the quadratic formula
    vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = h * h - a * c;

    if (discriminant < 0) {
        return -1.0;
    }
    else {
        // h - sqrt(h^2 - ac) / a
        return (h - std::sqrt(discriminant)) / a;
    }
}


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

    auto t = hit_sphere(point3(0, 0, -1), 0.5, r);

    if (t <= 0.0) {
        return BackgroundColor;
    }

    // Variables 

    vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
    vec3 L = vec3(0.0, 1.0, 0.0); // DirectionToLight

    double Kd = 0.7;
    double Ks = 0.2;
    double Ka = 0.1;
    double Kgls = 16.0;
    color Ia(0.0, 0.0, 0.0); // AmbientLight
    color Ip(1.0, 1.0, 1.0); // LightColor
    color Od(1.0, 0.0, 1.0);
    color Os(1.0, 1.0, 1.0); // Specular light

    // Calaculate refection direction R

    vec3 R = 2 * N * dot(N, L) - L; // reflect direction

    // Calculate illumination I

    color ambientTerm = Ka * Ia * Od;
    color diffuseTerm = Kd * Ip * Od * std::fmax(0.0, dot(N, L));
    color specularTerm = Ks * Ip * Os * std::pow(std::fmax(0.0, dot(R, L)), Kgls);
    color I = ambientTerm + diffuseTerm + specularTerm;

    return I;
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