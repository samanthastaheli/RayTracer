#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

color BackgroundColor = color(0.2, 0.2, 0.2);

class sphere {
public:
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0, radius)) {}

    auto hit(const ray& r) {
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

    color get_color(const ray& r) {
        auto t = hit(r);

        if (t <= 0.0) {
            return BackgroundColor;
        }


        // Variables 

        //vec3 N = unit_vector(r.at(t) - vec3(0, 0, -1));
        vec3 N = unit_vector(r.at(t) - center);
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

private:
    point3 center;
    double radius;
};

#endif