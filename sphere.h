#ifndef SPHERE_H
#define SPHERE_H

#include <limits>
#include <vector>
#include <memory>
#include "vec3.h"

double closest_t = std::numeric_limits<double>::infinity();
inline constexpr double infinity = std::numeric_limits<double>::infinity();
extern color BackgroundColor;
//using std::vector;
//extern std::vector<sphere> spheres;

class sphere {
private:
    point3 center;
    double radius, Kd, Ks, Ka, Kgls, Refl;
    color Od, Os;

public:
    sphere(const point3& center, double radius,
		double Kd, double Ks, double Ka, double Kgls, double Refl,
        color Od, color Os)
        : center(center),
        radius(std::fmax(0, radius)),
        Kd(Kd),
        Ks(Ks),
        Ka(Ka),
        Kgls(Kgls),
        Refl(Refl),
        Od(Od),
        Os(Os) {
    }
    auto hit(const ray& r) {
        // h = d * (C - Q)
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

    color trace_ray(ray r, int depth, const std::vector<std::shared_ptr<sphere>>& spheres) {
        if (depth <= 0)
            return color(0.0, 0.0, 0.0);

        double closest_t = infinity;
        sphere* hit_object = nullptr;

        // Find closest intersection
        for (auto& s : spheres) {
            double t = s->hit(r);

            if (t > 0.001 && t < closest_t) {
                closest_t = t;
                hit_object = s.get();
            }
        }

        // If nothing hit → background
        if (hit_object == nullptr) {
            return BackgroundColor;
        }

        // Compute shading
        return hit_object->get_color(r, closest_t, depth, spheres);
    }

    color get_color(const ray& r, double t, int depth, const std::vector<std::shared_ptr<sphere>>& spheres) {
        /*
        slide 12 and 13 in IlluminationShading
        Equations for I and R:
            I = ambientTerm + diffuseTerm + specularTerm
            ambientTerm = k_a * Ia * O_d
            diffuseTerm = k_d * I_p * O_d * max(0, N
            specularTerm = k_s * I_p * O_s * max(0, R dot O_s)^n

            R = reflect direction 2N(N dot L) - L

            Specular light = Os
        */

        if (t <= 0.0) {
            return BackgroundColor;
        }

        // Variables 

        vec3 N = unit_vector(r.at(t) - center);
        vec3 L = unit_vector(vec3(0.0, 3.0, 2.0) - r.at(t)); // DirectionToLight
        color Ia(0.0, 0.0, 0.0); // AmbientLight
        color Ip(1.0, 1.0, 1.0); // LightColor

        // Calaculate refection direction R

        vec3 R = 2 * N * dot(N, L) - L; // reflect direction

		// Create reflection ray

		double offset = 0.001; // must have small offset to avoid self-intersection
		point3 hitPoint = r.at(t) + offset * N;
		ray reflectionRay(hitPoint, R);
        color reflectionColor = trace_ray(reflectionRay, 3, spheres);

        // Calculate illumination I

        color ambientTerm = Ka * Ia * Od;
        color diffuseTerm = Kd * Ip * Od * std::fmax(0.0, dot(N, L));
        color specularTerm = Ks * Ip * Os * std::pow(std::fmax(0.0, dot(R, L)), Kgls);
        color I = ambientTerm + diffuseTerm + specularTerm + Refl * reflectionColor;

        return I;
    }
};

#endif