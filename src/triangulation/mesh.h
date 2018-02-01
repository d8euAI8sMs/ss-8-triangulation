#pragma once

#include <util/common/plot/shape.h>

#include <vector>

namespace plot
{

    struct triangle
    {
        size_t i, j, k;
    };

    struct enclosing_circle
    {
        point < double > center;
        double radius_sq;
    };

    struct triangle_info
    {
        bool phantom;
        bool super;
        enclosing_circle enclosing;
    };

    struct vertex_info
    {
        bool super;
    };

    struct mesh
    {

        /* the index of triangles */
        std::vector < triangle > triangles;

        /* the vector of triangle details */
        std::vector < triangle_info > triangle_infos;

        /* the index of phantom triangles */
        std::vector < size_t > phantom_triangles;

        /* the index of vertices */
        std::vector < plot::point < double > > vertices;

        /* the vector of vertex details */
        std::vector < vertex_info > vertex_infos;

        /* the vector of neighbor vertices of the given vertex */
        std::vector < std::vector < size_t > > neighbor_vertices;

        /* the vector of neighbor triangles of the given vertex */
        std::vector < std::vector < size_t > > neighbor_triangles;
    };

    inline static enclosing_circle get_enclosing_circle
    (
        const mesh & m,
        const triangle & t
    )
    {
        auto p1 = m.vertices[t.i];
        auto p2 = m.vertices[t.j];
        auto p3 = m.vertices[t.k];

        double d = 2 *
        (
            (p2.x - p1.x) * (p3.y - p1.y) -
            (p3.x - p1.x) * (p2.y - p1.y)
        );

        point < double > c;

        c.x =
        (
            p2.x * p2.x - p1.x * p1.x +
            p2.y * p2.y - p1.y * p1.y
        ) * (p3.y - p1.y) -
        (
            p3.x * p3.x - p1.x * p1.x +
            p3.y * p3.y - p1.y * p1.y
        ) * (p2.y - p1.y);

        c.y =
        (
            p3.x * p3.x - p1.x * p1.x +
            p3.y * p3.y - p1.y * p1.y
        ) * (p2.x - p1.x) -
        (
            p2.x * p2.x - p1.x * p1.x +
            p2.y * p2.y - p1.y * p1.y
        ) * (p3.x - p1.x);

        c.x /= d;
        c.y /= d;

        double r_sq =
            (c.x - p1.x) * (c.x - p1.x) +
            (c.y - p1.y) * (c.y - p1.y);

        return
        {
            c,
            r_sq
        };
    }
}
