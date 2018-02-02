#pragma once

#include <util/common/plot/shape.h>

#include <vector>
#include <set>
#include <array>
#include <algorithm>
#include <numeric>

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
        std::vector < std::set < size_t > > neighbor_vertices;

        /* the vector of neighbor triangles of the given vertex */
        std::vector < std::set < size_t > > neighbor_triangles;
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

        if (d == 0)
        {
            return { { 0, 0 }, 0 };
        }

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

    inline static bool is_in_circle
    (
        const enclosing_circle & c,
        const plot::point < double > & p
    )
    {
        return
        (
            (
                (c.center.x - p.x) * (c.center.x - p.x) +
                (c.center.y - p.y) * (c.center.y - p.y)
            ) - c.radius_sq
        ) < -1e-8;
    }

    inline static bool is_on_circle
    (
        const enclosing_circle & c,
        const plot::point < double > & p
    )
    {
        return std::abs
        (
            (
                (c.center.x - p.x) * (c.center.x - p.x) +
                (c.center.y - p.y) * (c.center.y - p.y)
            ) - c.radius_sq
        ) <= 1e-8;
    }

    inline static bool is_in_or_on_circle
    (
        const enclosing_circle & c,
        const plot::point < double > & p
    )
    {
        return
        (
            (
                (c.center.x - p.x) * (c.center.x - p.x) +
                (c.center.y - p.y) * (c.center.y - p.y)
            ) - c.radius_sq
        ) <= 1e-8;
    }

    inline static bool _intersects
    (
        point < double > p1, point < double > p2,
        point < double > p3, point < double > p4
    )
    {
        /* check if line segments are same */
        if ((p1.x == p3.x) && (p1.y == p3.y) &&
            (p2.x == p4.x) && (p2.y == p4.y)) return false;
        if ((p1.x == p4.x) && (p1.y == p4.y) &&
            (p2.x == p3.x) && (p2.y == p3.y)) return false;

        double d =
            (p4.y - p3.y) * (p2.x - p1.x) -
            (p4.x - p3.x) * (p2.y - p1.y);

        double n1 =
            (p4.x - p3.x) * (p1.y - p3.y) -
            (p4.y - p3.y) * (p1.x - p3.x);
        double n2 =
            (p2.x - p1.x) * (p1.y - p3.y) -
            (p2.y - p1.y) * (p1.x - p3.x);

        double q1 = n1 / d;
        double q2 = n2 / d;

        if (isfinite(q1) && isfinite(q2) &&
            (1e-8 < q1) && (q1 < (1 - 1e-8)) &&
            (1e-8 < q2) && (q2 < (1 - 1e-8)))
        {
            return true;
        }

        return false;
    }

    inline static bool intersects
    (
        const mesh & m,
        const triangle & t1,
        const triangle & t2
    )
    {
        if (_intersects(
            m.vertices[t1.i], m.vertices[t1.j],
            m.vertices[t2.i], m.vertices[t2.j])) return true;
        if (_intersects(
            m.vertices[t1.i], m.vertices[t1.j],
            m.vertices[t2.i], m.vertices[t2.k])) return true;
        if (_intersects(
            m.vertices[t1.i], m.vertices[t1.j],
            m.vertices[t2.j], m.vertices[t2.k])) return true;
        if (_intersects(
            m.vertices[t1.i], m.vertices[t1.k],
            m.vertices[t2.i], m.vertices[t2.j])) return true;
        if (_intersects(
            m.vertices[t1.i], m.vertices[t1.k],
            m.vertices[t2.i], m.vertices[t2.k])) return true;
        if (_intersects(
            m.vertices[t1.i], m.vertices[t1.k],
            m.vertices[t2.j], m.vertices[t2.k])) return true;
        if (_intersects(
            m.vertices[t1.j], m.vertices[t1.k],
            m.vertices[t2.i], m.vertices[t2.j])) return true;
        if (_intersects(
            m.vertices[t1.j], m.vertices[t1.k],
            m.vertices[t2.i], m.vertices[t2.k])) return true;
        if (_intersects(
            m.vertices[t1.j], m.vertices[t1.k],
            m.vertices[t2.j], m.vertices[t2.k])) return true;
        return false;
    }

    inline static size_t _insert_triangle
    (
        mesh & m,
        const triangle & t,
        const triangle_info & ti
    )
    {
        size_t t_idx = m.triangles.size();
        if (!m.phantom_triangles.empty())
        {
            t_idx = m.phantom_triangles.back();
            m.phantom_triangles.pop_back();
            m.triangles[t_idx] = t;
            m.triangle_infos[t_idx] = ti;
        }
        else
        {
            m.triangles.push_back(t);
            m.triangle_infos.push_back(ti);
        }
        m.neighbor_vertices[t.i].insert(t.j);
        m.neighbor_vertices[t.i].insert(t.k);
        m.neighbor_vertices[t.j].insert(t.i);
        m.neighbor_vertices[t.j].insert(t.k);
        m.neighbor_vertices[t.k].insert(t.i);
        m.neighbor_vertices[t.k].insert(t.j);
        m.neighbor_triangles[t.i].insert(t_idx);
        m.neighbor_triangles[t.j].insert(t_idx);
        m.neighbor_triangles[t.k].insert(t_idx);

        return t_idx;
    }

    inline static void _delete_triangle
    (
        mesh & m,
        size_t t_idx
    )
    {
        auto & t = m.triangles[t_idx];
        m.triangle_infos[t_idx].phantom = true;
        m.phantom_triangles.push_back(t_idx);
        m.neighbor_triangles[t.i].erase(t_idx);
        m.neighbor_triangles[t.j].erase(t_idx);
        m.neighbor_triangles[t.k].erase(t_idx);
        m.neighbor_vertices[t.i].erase(t.j);
        m.neighbor_vertices[t.i].erase(t.k);
        m.neighbor_vertices[t.j].erase(t.i);
        m.neighbor_vertices[t.j].erase(t.k);
        m.neighbor_vertices[t.k].erase(t.i);
        m.neighbor_vertices[t.k].erase(t.j);
    }

    inline static void _triangulate
    (
        mesh & m,
        const std::vector < size_t > & vertices
    )
    {
        ASSERT(vertices.size() >= 3);

        triangle t;
        triangle_info ti;
        ti.super = false;
        ti.phantom = false;

        std::vector < size_t > circle_collision_triangles;

        for (size_t i = 0;     i < vertices.size(); ++i)
        for (size_t j = i + 1; j < vertices.size(); ++j)
        for (size_t k = j + 1; k < vertices.size(); ++k)
        {
            t = { vertices[i], vertices[j], vertices[k] };
            ti.enclosing = get_enclosing_circle(m, t);

            if (ti.enclosing.radius_sq == 0) continue;

            bool satisfies = true;
            bool circle_collision = false;

            for (size_t l = 0; l < m.vertices.size(); ++l)
            {
                if ((l == t.i) || (l == t.j) || (l == t.k)) continue;
                if (is_in_circle(ti.enclosing, m.vertices[l]))
                {
                    satisfies = false;
                    break;
                }
                if (is_on_circle(ti.enclosing, m.vertices[l]))
                {
                    circle_collision = true;
                    continue;
                }
            }

            if (satisfies)
            {
                bool can_insert = true;
                if (circle_collision)
                {
                    auto & c1 = ti.enclosing;
                    for (size_t l = 0;
                         l < circle_collision_triangles.size();
                         ++l)
                    {
                        auto & c2 = m.triangle_infos[circle_collision_triangles[l]].enclosing;
                        if ((std::abs(c1.center.x - c2.center.x) <= 1e-8) &&
                            (std::abs(c1.center.y - c2.center.y) <= 1e-8) &&
                            (std::abs(c1.radius_sq - c2.radius_sq) <= 1e-8))
                        {
                            if (intersects(m, t, m.triangles[circle_collision_triangles[l]]))
                            {
                                can_insert = false;
                                break;
                            }
                        }
                    }
                }
                if (can_insert)
                {
                    auto criteria = [&m, &t] (size_t l)
                    {
                        return intersects(m, t, m.triangles[l]);
                    };
                    can_insert = std::none_of
                    (
                        m.neighbor_triangles[t.i].begin(),
                        m.neighbor_triangles[t.i].end(),
                        criteria
                    );
                    if (!can_insert) continue;
                    can_insert = std::none_of
                    (
                        m.neighbor_triangles[t.j].begin(),
                        m.neighbor_triangles[t.j].end(),
                        criteria
                    );
                    if (!can_insert) continue;
                    can_insert = std::none_of
                    (
                        m.neighbor_triangles[t.k].begin(),
                        m.neighbor_triangles[t.k].end(),
                        criteria
                    );
                    if (!can_insert) continue;
                }
                if (can_insert)
                {
                    ti.super = m.vertex_infos[t.i].super
                             | m.vertex_infos[t.j].super
                             | m.vertex_infos[t.k].super;
                    size_t t_idx = _insert_triangle(m, t, ti);
                    if (circle_collision)
                    {
                        circle_collision_triangles.push_back(t_idx);
                    }
                }
            }
        }
    }

    inline static void init_mesh
    (
        mesh & m,
        const std::vector < point < double > > & vertices
    )
    {
        m.vertices.insert(m.vertices.end(),
                          vertices.begin(),
                          vertices.end());
        m.vertex_infos.resize(vertices.size(), vertex_info { true });
        m.neighbor_vertices.resize(vertices.size());
        m.neighbor_triangles.resize(vertices.size());

        std::vector < size_t > indices(vertices.size());
        std::iota(indices.begin(), indices.end(), 0);
        _triangulate(m, indices);
    }

    inline static void add_to_mesh
    (
        mesh & m,
        const point < double > & vertex
    )
    {
        std::set < size_t > indices;

        for (size_t i = 0; i < m.triangles.size(); ++i)
        {
            if (m.triangle_infos[i].phantom) continue;
            if (is_in_or_on_circle(m.triangle_infos[i].enclosing, vertex))
            {
                auto & t = m.triangles[i];
                _delete_triangle(m, i);
                indices.insert(t.i);
                indices.insert(t.j);
                indices.insert(t.k);
            }
        }

        if (indices.empty()) return;

        m.vertices.push_back(vertex);
        m.vertex_infos.push_back(vertex_info { false });
        m.neighbor_vertices.emplace_back();
        m.neighbor_triangles.emplace_back();

        indices.insert(m.vertices.size() - 1);

        _triangulate(m, std::vector < size_t > (indices.begin(), indices.end()));
    }

    inline static void add_to_mesh
    (
        mesh & m,
        const std::vector < point < double > > & vertices
    )
    {
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            add_to_mesh(m, vertices[i]);
        }
    }
}
