#pragma once

#include <vector>

#include <util/common/ptr.h>
#include <util/common/iterable.h>
#include <util/common/plot/palette.h>
#include <util/common/plot/drawable.h>
#include <util/common/plot/data_source.h>
#include <util/common/plot/custom_drawable.h>
#include <util/common/plot/utility.h>

#include "mesh.h"

namespace plot
{

    class mesh_drawable : public drawable
    {

    public:

        using ptr_t = util::ptr_t < mesh_drawable > ;

        template < class ... T > static ptr_t create(T && ... t) { return util::create < typename ptr_t::element_type > (std::forward < T > (t) ...); }

    public:

        data_source_t < mesh > data_source;
        drawable::ptr_t  point_painter;
        palette::pen_ptr line_pen;
        bool superstructure_visible;

    public:

        mesh_drawable()
        {
        }

        mesh_drawable
        (
            data_source_t < mesh > data_source,
            drawable::ptr_t  point_painter = custom_drawable::create(circle_painter()),
            palette::pen_ptr line_pen = palette::pen()
        )
            : data_source(std::move(data_source))
            , point_painter(std::move(point_painter))
            , line_pen(line_pen)
            , superstructure_visible(false)
        {
        }

        virtual ~mesh_drawable()
        {
        }

    protected:

        virtual void do_draw(CDC &dc, const viewport &bounds) override
        {
            if (!data_source) return;
            auto m = data_source(bounds);
            if (!m || m->triangles.empty()) return;

            auto & data = *m;

            for (size_t i = 0; i < data.triangles.size(); ++i)
            {
                if (data.triangle_infos[i].phantom
                    || !superstructure_visible && data.triangle_infos[i].super)
                {
                    continue;
                }

                if (line_pen)
                {
                    auto old = dc.SelectObject(line_pen.get());
                    dc.MoveTo(bounds.world_to_screen().xy(data.vertices[data.triangles[i].i]));
                    dc.LineTo(bounds.world_to_screen().xy(data.vertices[data.triangles[i].j]));
                    dc.LineTo(bounds.world_to_screen().xy(data.vertices[data.triangles[i].k]));
                    dc.LineTo(bounds.world_to_screen().xy(data.vertices[data.triangles[i].i]));
                    dc.SelectObject(old);
                }

                if (point_painter)
                {
                    point_painter->draw_at(dc, bounds, data.vertices[data.triangles[i].i]);
                    point_painter->draw_at(dc, bounds, data.vertices[data.triangles[i].j]);
                    point_painter->draw_at(dc, bounds, data.vertices[data.triangles[i].k]);
                }
            }
        }
    };
}
