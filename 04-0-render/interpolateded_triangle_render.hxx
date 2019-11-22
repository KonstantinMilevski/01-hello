#include "indexed_triangle_render.hxx"

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

double interpolate(const double& first, const double& second, const double& t);
vertex interpolate(const vertex& first, const vertex& second, const double& t);
bool   operator==(const vertex& first, const vertex& second);

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

void draw_cells(const size_t& row, const size_t& col,
                std::vector<vertex>& vertexes, std::vector<uint8_t>& indexes);

struct gfx_program
{
    virtual void   set_uniforms(const uniforms&)       = 0;
    virtual color  fragment_shader(const vertex& v_in) = 0;
    virtual vertex vertex_shader(const vertex& v_in)   = 0;
    ~gfx_program()                                     = default;
};

struct interpolateded_triangle_render : indexed_triangle_render
{
    interpolateded_triangle_render(canvas& buffer, size_t width, size_t height);
    void set_gfx_program(gfx_program& program);
    void draw_interpolated_triangle(const std::vector<vertex>&  vertexes,
                                    const std::vector<uint8_t>& indexes);

private:
    void raster_one_horizontal_line(const vertex& left, const vertex& right,
                                    std::vector<vertex>& out);
    std::vector<vertex> raster_horizontal_triangle(const vertex& single,
                                                   const vertex& left,
                                                   const vertex& right);

    std::vector<vertex> rasterize_triangle(const vertex& v0, const vertex& v1,
                                           const vertex& v2);
    gfx_program*        program_ = nullptr;
};
