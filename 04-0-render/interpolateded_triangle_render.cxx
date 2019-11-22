#include "interpolateded_triangle_render.hxx"
#include <algorithm>

interpolateded_triangle_render::interpolateded_triangle_render(canvas& buffer,
                                                               size_t  width,
                                                               size_t  height)
    : indexed_triangle_render(buffer, width, height)
{
}

void interpolateded_triangle_render::set_gfx_program(gfx_program& program)
{
    program_ = &program;
}

void interpolateded_triangle_render::draw_interpolated_triangle(
    const std::vector<vertex>& vertexes, const std::vector<uint8_t>& indexes)
{
    // std::vector<vertex>& //vertexes
    // ?check size
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
        vertex v0 = vertexes[indexes.at(i)];
        vertex v1 = vertexes[indexes.at(i + 1)];
        vertex v2 = vertexes[indexes.at(i + 2)];

        vertex v0_ = program_->vertex_shader(v0);
        vertex v1_ = program_->vertex_shader(v1);
        vertex v2_ = program_->vertex_shader(v2);

        std::vector<vertex> interpolated = rasterize_triangle(v0_, v1_, v2_);

        for (auto interpolated_vertex : interpolated)
        {
            const color c     = program_->fragment_shader(interpolated_vertex);
            position    pixel = { static_cast<int32_t>(interpolated_vertex.f0),
                               static_cast<int32_t>(interpolated_vertex.f1) };
            line_render::set_pixel(pixel, c);
        }
    }
}
void interpolateded_triangle_render::raster_one_horizontal_line(
    const vertex& left, const vertex& right, std::vector<vertex>& out)
{

    size_t num_of_points =
        static_cast<size_t>(std::round(std::abs(right.f0 - left.f0)));
    if (num_of_points > 0)
    {

        for (size_t i = 0; i <= (num_of_points + 1); ++i)
        {
            double index_t = static_cast<double>(i) / (num_of_points + 1);
            vertex point   = interpolate(left, right, index_t);
            out.push_back(point);
        }
    }
    else
    {
        out.push_back(left);
    }
}

std::vector<vertex> interpolateded_triangle_render::raster_horizontal_triangle(
    const vertex& single, const vertex& left, const vertex& right)
{
    std::vector<vertex> out;

    size_t num_points_in_line =
        static_cast<size_t>(std::round(std::abs(left.f1 - single.f1)));
    if (num_points_in_line > 0)
    {
        for (size_t i = 0; i <= (num_points_in_line + 1); ++i)
        {
            double index_t = static_cast<double>(i) / (num_points_in_line + 1);
            vertex left_point  = interpolate(left, single, index_t);
            vertex right_point = interpolate(right, single, index_t);
            raster_one_horizontal_line(left_point, right_point, out);
        }
    }
    else
    {
        raster_one_horizontal_line(left, right, out);
    }

    return out;
}

std::vector<vertex> interpolateded_triangle_render::rasterize_triangle(
    const vertex& v0, const vertex& v1, const vertex& v2)
{

    std::vector<vertex>          out;
    std::array<const vertex*, 3> tops{ &v0, &v1, &v2 };
    std::sort(begin(tops), end(tops), [](const vertex* v0, const vertex* v1) {
        return (v0->f1 < v1->f1);
    });
    const vertex& top    = *(tops.at(0));
    const vertex& middle = *(tops.at(1));
    const vertex& bottom = *(tops.at(2));

    // check points on horizontal line, TRUE - find central vertex & draw
    // horizontal triangle

    if (top.f1 == middle.f1 || middle.f1 == bottom.f1 || top.f1 == bottom.f1)
    {
        std::array<const vertex*, 3> tops_in_line{ &top, &middle, &bottom };
        std::sort(begin(tops_in_line), end(tops_in_line),
                  [](const vertex* v0, const vertex* v1) {
                      return (v0->f0 < v1->f0);
                  });
        const vertex& left   = *(tops_in_line.at(0));
        const vertex& center = *(tops_in_line.at(1));
        const vertex& right  = *(tops_in_line.at(2));

        out = raster_horizontal_triangle(center, left, right);
        return out;
    }

    /// find middle position on the long side

    const double length_top_bottom =
        std::sqrt((top.f0 - bottom.f0) * (top.f0 - bottom.f0) +
                  (top.f1 - bottom.f1) * (top.f1 - bottom.f1));
    vertex second_middle_vertex{ 0, 0, 0, 0, 0, 0, 0, 0 };
    if (0 == length_top_bottom)
    {
        out.push_back(top);
        return out;
    }
    else
    {

        const double length_bottom_middle =
            length_top_bottom * (top.f1 - middle.f1) / (top.f1 - bottom.f1);
        double t = length_bottom_middle / length_top_bottom;

        second_middle_vertex = interpolate(top, bottom, t);
    }
    std::vector<vertex> top_triangle =
        raster_horizontal_triangle(top, middle, second_middle_vertex);
    std::vector<vertex> bottom_triangle =
        raster_horizontal_triangle(bottom, middle, second_middle_vertex);

    out.insert(std::end(out), begin(top_triangle), std::end(top_triangle));
    out.insert(std::end(out), begin(bottom_triangle),
               std::end(bottom_triangle));

    return out;
}

bool operator==(const vertex& first, const vertex& second)
{
    return (first.f0 == second.f0 && first.f1 == second.f1 &&
            first.f2 == second.f2 && first.f3 == second.f3 &&
            first.f4 == second.f4 && first.f5 == second.f5 &&
            first.f6 == second.f6 && first.f7 == second.f7);
}

double interpolate(const double& first, const double& second, const double& t)
{
    assert((t >= 0.0, "Index less 0"));
    assert((t <= 1.0, "Index more 1"));
    return (first + (second - first) * t);
}

vertex interpolate(const vertex& first, const vertex& second, const double& t)
{
    return {
        first.f0 + (second.f0 - first.f0) * t,
        first.f1 + (second.f1 - first.f1) * t,
        first.f2 + (second.f2 - first.f2) * t,
        first.f3 + (second.f3 - first.f3) * t,
        first.f4 + (second.f4 - first.f4) * t,
        first.f5 + (second.f5 - first.f5) * t,
        first.f6 + (second.f6 - first.f6) * t,
        first.f7 + (second.f7 - first.f7) * t,
    };
}

void draw_cells(const size_t& max_x, const size_t& max_y,
                std::vector<vertex>&  triangles_for_index,
                std::vector<uint8_t>& indexes)
{

    int32_t step_x = (width - 1) / max_x;
    int32_t step_y = (height - 1) / max_y;

    for (size_t i = 0; i <= max_y; ++i)
    {
        for (size_t j = 0; j <= max_x; ++j)
        {
            vertex v{ static_cast<double>(j) * step_x,
                      static_cast<double>(i) * step_y,
                      0,
                      1,
                      0,
                      319,
                      239,
                      0 };

            triangles_for_index.push_back(v);
        }
    }

    assert(triangles_for_index.size() == (max_x + 1) * (max_y + 1));
    //<uint16_t>
    for (size_t y = 0; y < max_y; ++y)
    {
        for (size_t x = 0; x < max_x; ++x)
        {
            int     a      = x + (max_x + 1) * y;
            uint8_t index0 = static_cast<uint8_t>(x + (max_x + 1) * y);
            assert((x + (max_x + 1) * y) < 256);
            uint8_t index1 = static_cast<uint8_t>(index0 + 1);
            uint8_t index2 = index1;
            indexes.push_back(index0);
            indexes.push_back(index1);
            indexes.push_back(index2);
            if (y == (max_y - 1))
            {
                uint8_t index0 =
                    static_cast<uint8_t>(x + (max_x + 1) * (y + 1));
                uint8_t index1 = static_cast<uint8_t>(index0 + 1);
                uint8_t index2 = index1;
                indexes.push_back(index0);
                indexes.push_back(index1);
                indexes.push_back(index2);
            }
        }

        for (size_t x = 0; x <= max_x; ++x)
        {
            for (size_t y = 0; y < max_y; ++y)
            {
                uint8_t index0 = static_cast<uint8_t>((max_x + 1) * y + x);
                assert((((max_x + 1) * y + x) > 256, "Index is more then 256"));
                uint8_t index3 = static_cast<uint8_t>(index0 + (max_x + 1));
                uint8_t index4 = index3;

                indexes.push_back(index0);
                indexes.push_back(index3);
                indexes.push_back(index4);
            }
        }
    }
