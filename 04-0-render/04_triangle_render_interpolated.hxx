#pragma once

#include "03_triangle_render_indexed.hxx"

struct vertex
{
    double f0 = 0; /// x
    double f1 = 0; /// y
    double f2 = 0; /// r
    double f3 = 0; /// g
    double f4 = 0; /// b
    double f5 = 0; /// u (texture coordinate)
    double f6 = 0; /// v (texture coordinate)
    double f7 = 0; /// ?
};

const double        interpolate(const double&, const double&, const double&);
vertex              interpolate(const vertex&, const vertex&, const double&);
std::vector<vertex> points_on_side(const vertex&, const vertex&);

struct uniforms
{
    double f0 = 0;
    double f1 = 0;
    double f2 = 0;
    double f3 = 0;
    double f4 = 0;
    double f5 = 0;
    double f6 = 0;
    double f7 = 0;
};

struct gfx_program
{
    virtual ~gfx_program()                        = default;
    virtual void   set_uniforms(const uniforms&)  = 0;
    virtual vertex vertex_shader(const vertex&)   = 0;
    virtual color  fragment_shader(const vertex&) = 0;
};

struct triangle_interpolated : triangle_indexed_render
{
    triangle_interpolated(canvas& buffer, size_t width, size_t height);
    void set_gfx_program(gfx_program& program) { program_ = &program; }
    void draw_filled_triangle(std::vector<vertex>&  vertexes,
                              std::vector<uint8_t>& indexes);
    void draw_empty_triangle(std::vector<vertex>&  vertexes,
                             std::vector<uint8_t>& indexes);

private:
    std::vector<vertex> rasterize_triangle(const vertex& v0, const vertex& v1,
                                           const vertex& v2);
    std::vector<vertex> rasterize_empty_triangle(const vertex& v0,
                                                 const vertex& v1,
                                                 const vertex& v2);
    std::vector<vertex> raster_horizontal_triangle(const vertex& single,
                                                   const vertex& left,
                                                   const vertex& right);

    void raster_one_horizontal_line(const vertex&        left_vertex,
                                    const vertex&        right_vertex,
                                    std::vector<vertex>& out);

    gfx_program* program_ = nullptr;
};
