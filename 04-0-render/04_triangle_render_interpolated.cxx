#include "04_triangle_render_interpolated.hxx"
#include <algorithm>
#include <iostream>
const double interpolate(const double& f0, const double& f1, const double& t)
{
    assert(t >= 0);
    assert(t <= 1);
    return f0 + (f1 - f0) * t;
}

vertex interpolate(const vertex& v0, const vertex& v1, const double& t)
{
    return { interpolate(v0.f0, v1.f0, t), interpolate(v0.f1, v1.f1, t),
             interpolate(v0.f2, v1.f2, t), interpolate(v0.f3, v1.f3, t),
             interpolate(v0.f4, v1.f4, t), interpolate(v0.f5, v1.f5, t),
             interpolate(v0.f6, v1.f6, t), interpolate(v0.f7, v1.f7, t) };
}

triangle_interpolated::triangle_interpolated(canvas& buffer, size_t width,
                                             size_t height)
    : triangle_indexed_render(buffer, width, height

      )
{
}

void triangle_interpolated::raster_one_horizontal_line(
    const vertex& left_vertex, const vertex& right_vertex,
    std::vector<vertex>& out)
{

    size_t num_pixel_in_line = static_cast<size_t>(
        std::round(std::abs(right_vertex.f0 - left_vertex.f0)));
    if (num_pixel_in_line > 0)
    {
        for (size_t p = 0; p <= num_pixel_in_line + 1; ++p)
        {
            double t_index = static_cast<double>(p) / (num_pixel_in_line + 1);
            vertex pixel   = interpolate(left_vertex, right_vertex, t_index);
            out.push_back(pixel);
        }
    }
    else
    {
        out.push_back(left_vertex);
    }
}

std::vector<vertex> triangle_interpolated::raster_horizontal_triangle(
    const vertex& single, const vertex& left, const vertex& right)
{
    std::vector<vertex> out;
    size_t              num_of_lines =
        static_cast<size_t>(std::round(std::abs(single.f1 - left.f1)));
    if (num_of_lines > 0)
    {
        for (size_t p = 0; p <= num_of_lines; ++p)
        {
            double t_index      = static_cast<double>(p) / (num_of_lines);
            vertex left_vertex  = interpolate(left, single, t_index);
            vertex right_vertex = interpolate(right, single, t_index);

            raster_one_horizontal_line(left_vertex, right_vertex, out);
        }
    }
    else
    {
        raster_one_horizontal_line(left, right, out);
    }
    return out;
}
//+
std::vector<vertex> triangle_interpolated::rasterize_triangle(const vertex& v0,
                                                              const vertex& v1,
                                                              const vertex& v2)
{
    std::vector<vertex>          out;
    std::array<const vertex*, 3> in_vertexes{ &v0, &v2, &v1 };
    std::sort(begin(in_vertexes), end(in_vertexes),
              [](const vertex* left, const vertex* right) {
                  return (left->f1 < right->f1);
              });
    const vertex&         top    = *in_vertexes.at(0);
    const vertex&         middle = *in_vertexes.at(1);
    const vertex&         bottom = *in_vertexes.at(2);
    position              start{ static_cast<int32_t>(std::round(top.f0)),
                    static_cast<int32_t>(std::round(top.f1)) };
    position              end{ static_cast<int32_t>(std::round(bottom.f0)),
                  static_cast<int32_t>(std::round(bottom.f1)) };
    std::vector<position> long_side_point = pixels_positions(start, end);

    auto it_middle = std::find_if(
        begin(long_side_point), std::end(long_side_point),
        [&](const position& pos) {
            return pos.y == static_cast<int32_t>(std::round(middle.f1));
        });
    assert(it_middle != std::end(long_side_point));

    const position second_middle = *it_middle;

    double t = (second_middle - start).length() / (end - start).length();

    vertex second_middle_vertex = interpolate(top, bottom, t);

    std::vector<vertex> top_triangle =
        raster_horizontal_triangle(top, middle, second_middle_vertex);
    std::vector<vertex> bottom_triangle =
        raster_horizontal_triangle(bottom, middle, second_middle_vertex);

    out.insert(std::end(out), begin(top_triangle), std::end(top_triangle));
    out.insert(std::end(out), begin(bottom_triangle),
               std::end(bottom_triangle));

    return out;
}
//+
void triangle_interpolated::draw_filled_triangle(std::vector<vertex>&  vertexes,
                                                 std::vector<uint8_t>& indexes)
{
    for (size_t index = 0; index < indexes.size(); index += 3)
    {
        const uint8_t index0 = indexes.at(index + 0);
        const uint8_t index1 = indexes.at(index + 1);
        const uint8_t index2 = indexes.at(index + 2);

        const vertex& v0 = vertexes.at(index0);
        const vertex& v1 = vertexes.at(index1);
        const vertex& v2 = vertexes.at(index2);

        const vertex v0_ = program_->vertex_shader(v0);
        const vertex v1_ = program_->vertex_shader(v1);
        const vertex v2_ = program_->vertex_shader(v2);

        const std::vector<vertex> interpoleted{ rasterize_triangle(v0_, v1_,
                                                                   v2_) };
        for (const vertex& interpolated_vertex : interpoleted)
        {
            const color    c = program_->fragment_shader(interpolated_vertex);
            const position pos{
                static_cast<int32_t>(std::round(interpolated_vertex.f0)),
                static_cast<int32_t>(std::round(interpolated_vertex.f1))
            };
            set_pixel(pos, c);
        }
    }
}

void draw_line(const vertex& first, const vertex& second,
               std::vector<vertex>& tops, std::vector<uint8_t>& indexes)
{

    vertex third = second;
    tops.push_back(first);
    tops.push_back(second);
    tops.push_back(third);
    indexes = { 0, 1, 2 };
}
