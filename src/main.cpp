#include <iostream>
#include <vector>
#include <array>
#include <random>

#include <c2ga/Mvec.hpp>

using Mvec = c2ga::Mvec<double>;
const Mvec e_o = c2ga::e0<double>();
const Mvec e_1 = c2ga::e1<double>();
const Mvec e_2 = c2ga::e2<double>();
const Mvec e_i = c2ga::ei<double>();

/* geometric primitives */
namespace prim
{
    Mvec vector(const double x, const double y)
    {
        return x*e_1 + y*e_2;
    }
    Mvec point(const double x, const double y)
    {
        return  e_o + vector(x,y) + 0.5*(x*x+y*y)*e_i;
    }
    Mvec sphere(const double x, const double y, const double r)
    {
        return  (point(x,y) - 0.5*r*r*e_i).dual();
    }
}

struct Rng
{
    std::mt19937 gen;
    std::uniform_real_distribution<double> distrib;

    Rng(const uint64_t seed):
        gen(seed),
        distrib(0.0, 1.0)
    {}

    double
    scalar(const double min, const double max)
    {
        return distrib(gen)*(max-min) + min;
    }
    double
    scalar(const double radius=1.0)
    {
        return scalar(-radius, radius);
    }

    Mvec
    vector(const double radius=1.0, const bool normed=false) 
    {
        const auto vec = prim::vector(scalar(radius), scalar(radius));
        return vec / ((normed)? std::sqrt(vec|vec) : 1.0);
    }
    Mvec
    point(const double radius=1.0) 
    {
        return prim::point(scalar(radius), scalar(radius));
    }
    /* only generates real spheres */
    Mvec
    sphere(const double radius=1.0) 
    {
        return prim::sphere(scalar(radius), scalar(radius), scalar(0.0,radius));
    }

    Mvec
    sample_vec_in_space(const Mvec & space)
    {
        auto u = vector(1.0, true);
        u = space | u | space;
        const double mag = u | u;
        if (mag < 0.001)
        {
            return sample_vec_in_space(space);
        }
        else
        {
            return u / std::sqrt(mag);
        }
    }


    /** generates a random 3D vector basis */
    std::array<Mvec, 2>
    basis()
    {
        auto space = e_1^e_2;
        const auto u = sample_vec_in_space(space); space = space | u;
        auto v = space;

        v *= - (u^v) | (e_1^e_2);

        return   {u,v};
    }

};

Mvec
rounded(const Mvec & m)
{
    auto oui = m;
    oui.roundZero();
    return oui;
}

/** Creates the potential interfaces between every centroids.
    Most of them are to be filtered out.
    The centroids are considered to be of the form w²C with C a canonic circle (ie with its e0 coordinate equal to 1).
    Planes as centroids are not supported by that way of storing the weight.
*/
std::vector<Mvec>
create_interfaces(const std::vector<Mvec> & centroids)
{
    std::vector<Mvec> interfaces;
    for (int i = 0; i < centroids.size(); ++i)
    {
        for (int j = 0; j < centroids.size(); ++j)
        {
            if (i != j)
            {
                const auto & Ca = centroids[i];
                const auto & Cb = centroids[j];
                interfaces.push_back(Ca - Cb);
            }
        }
    }
    return interfaces;
}

#include "svg.hpp"

void
test_export()
{
    Rng rng(1231235552);
    Svg svg("test.svg");

    const auto p1 = rng.point(50.);
    const auto p2 = rng.point(50.);
    const auto p3 = rng.point(50.);
    const auto C = p1 ^ p2 ^ p3;

    const auto pen = (p1^p2).dual();
    auto flat = pen ^ e_i; flat /= std::sqrt(flat|flat);
    auto small = pen ^ flat.dual(); small /= - small.dual() | e_i;
    std::cout << "small = " << small.dual() << std::endl;
    std::cout << "flat = " << flat.dual() << std::endl;

    svg.cga(C);

    // svg.cga(flat);
    // svg.cga(small);
    int n = 100;
    for (int i = 0; i < n; ++i)
    {
        const float t = float(i)*M_PI/float(n);
        const Mvec oui = (std::cos(t)*p1 + std::sin(t)*p2).dual();
        svg.cga(oui);
    }
    
    svg.cga(p1)
        .cga(p2)
        .cga(p3);
    
}


void
run()
{
    Rng rng(1231235552);
    Svg svg("voronoi.svg");
    int n = 5;
    std::vector<Mvec> centroids;
    for (int i = 0; i < n; ++i)
    {
        centroids.push_back(
            (rng.point(50.0) - 0.5*rng.scalar(10.0)*e_i).dual()
            //  *(1.0+rng.scalar(0.1))
            );
    }

    auto interfaces = create_interfaces(centroids);
    for (const auto & faces: interfaces)
    {
        svg.cga(faces);
    }

    for (const auto & c: centroids)
    {
        svg.cga(c);
    }
}


int
main()
{

    test_export();
    run();
    std::cout << "Wa-houuuu !" << std::endl;
    return 0;
}
