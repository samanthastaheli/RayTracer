#ifndef POLYGON_H
#define POLYGON_H

#include "vec3.h"

extern color BackgroundColor;

class polygon {
private:
    point3 v0, v1, v2;
    double Kd, Ks, Ka, Kgls, Refl;
    color Od, Os;

public:
    polygon(point3 a, point3 b, point3 c,
        double Kd, double Ks, double Ka, double Kgls, double Refl,
        color Od, color Os)
        : v0(a), v1(b), v2(c),
        Kd(Kd), Ks(Ks), Ka(Ka), Kgls(Kgls), Refl(Refl),
        Od(Od), Os(Os) {
    }
    auto hit(const ray& r) {
        // project polygon onto 2D plane
		// translate polygon so intersection point is at origin
		// send ray down one coordinate and count number intersections of ray with the polygon
        const double EPSILON = 1e-8;

        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;

        vec3 h = cross(r.direction(), edge2);
        double a = dot(edge1, h);

        if (fabs(a) < EPSILON)
            return -1.0; // ray parallel to triangle

        double f = 1.0 / a;
        vec3 s = r.origin() - v0;

        double u = f * dot(s, h);
        if (u < 0.0 || u > 1.0)
            return -1.0;

        vec3 q = cross(s, edge1);

        double v = f * dot(r.direction(), q);
        if (v < 0.0 || u + v > 1.0)
            return -1.0;

        double t = f * dot(edge2, q);

        if (t > EPSILON)
            return t;

        return -1.0;
    }

    color get_color(const ray& r, double t, int depth) {
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

        vec3 N = unit_vector(cross(v1 - v0, v2 - v0));
        vec3 L = unit_vector(vec3(0.0, 3.0, 2.0) - r.at(t)); // DirectionToLight
        color Ia(0.0, 0.0, 0.0); // AmbientLight
        color Ip(1.0, 1.0, 1.0); // LightColor

        // Calaculate refection direction R

        vec3 R = 2 * N * dot(N, L) - L; // reflect direction

        // Calculate illumination I

        color ambientTerm = Ka * Ia * Od;
        color diffuseTerm = Kd * Ip * Od * std::fmax(0.0, dot(N, L));
        color specularTerm = Ks * Ip * Os * std::pow(std::fmax(0.0, dot(R, -unit_vector(r.direction()))), Kgls);
        color I = ambientTerm + diffuseTerm + specularTerm;

        return I;
    }
};

#endif