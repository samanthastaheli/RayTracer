#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"

color BackgroundColor = color(0.2, 0.2, 0.2);

class sphere {
private:
    point3 center;
    double radius;

    double Kd;
    double Ks;
    double Ka;
    double Kgls;
    color Od;
    color Os;
public:
    sphere(const point3& center, double radius,
        double Kd, double Ks, double Ka, double Kgls,
        color Od, color Os)
        : center(center),
        radius(std::fmax(0, radius)),
        Kd(Kd),
        Ks(Ks),
        Ka(Ka),
        Kgls(Kgls),
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

    color get_color(const ray& r) {
        // Specular light = Os
        auto t = hit(r);

        if (t <= 0.0) {
            return BackgroundColor;
        }

        // Variables 

        vec3 N = unit_vector(r.at(t) - center);
        vec3 L = vec3(0.0, 1.0, 0.0); // DirectionToLight
        color Ia(0.0, 0.0, 0.0); // AmbientLight
        color Ip(1.0, 1.0, 1.0); // LightColor

        // Calaculate refection direction R

        vec3 R = 2 * N * dot(N, L) - L; // reflect direction

        // Calculate illumination I

        color ambientTerm = Ka * Ia * Od;
        color diffuseTerm = Kd * Ip * Od * std::fmax(0.0, dot(N, L));
        color specularTerm = Ks * Ip * Os * std::pow(std::fmax(0.0, dot(R, L)), Kgls);
        color I = ambientTerm + diffuseTerm + specularTerm;

        return I;
    }
};

#endif