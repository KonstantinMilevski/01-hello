#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "glad/glad.h"
#include <SDL2/SDL.h>

#define GL_CHECK()                                                             \
    {                                                                          \
        const int err = static_cast<int>(glGetError());                        \
        if (err != GL_NO_ERROR)                                                \
        {                                                                      \
            switch (err)                                                       \
            {                                                                  \
                case GL_INVALID_ENUM:                                          \
                    std::cerr << GL_INVALID_ENUM << std::endl;                 \
                    break;                                                     \
                case GL_INVALID_VALUE:                                         \
                    std::cerr << GL_INVALID_VALUE << std::endl;                \
                    break;                                                     \
                case GL_INVALID_OPERATION:                                     \
                    std::cerr << GL_INVALID_OPERATION << std::endl;            \
                    break;                                                     \
                case GL_INVALID_FRAMEBUFFER_OPERATION:                         \
                    std::cerr << GL_INVALID_FRAMEBUFFER_OPERATION              \
                              << std::endl;                                    \
                    break;                                                     \
                case GL_OUT_OF_MEMORY:                                         \
                    std::cerr << GL_OUT_OF_MEMORY << std::endl;                \
                    break;                                                     \
            }                                                                  \
            assert(false);                                                     \
        }                                                                      \
    }

template <typename T>
static void load_gl_func(const char* func_name, T& result)
{
    void* gl_pointer = SDL_GL_GetProcAddress(func_name);
    if (nullptr == gl_pointer)
    {
        throw std::runtime_error(std::string("can't load GL function") +
                                 func_name);
    }
    result = reinterpret_cast<T>(gl_pointer);
}
constexpr int width = 640; // 640;
constexpr int heigh = 480; // 480;

/// for SDLevent
enum class event
{
    /// input events
    left_pressed,
    left_released,
    right_pressed,
    right_released,
    up_pressed,
    up_released,
    down_pressed,
    down_released,
    select_pressed,
    select_released,
    start_pressed,
    start_released,
    button1_pressed,
    button1_released,
    button2_pressed,
    button2_released,
    /// virtual console events
    turn_off
};
enum class keys
{
    left,
    right,
    down,
    rotate,
    pause,
    exit
};

struct vec2
{
    vec2() {}
    vec2(float x_, float y_)
        : x(x_)
        , y(y_)
    {
    }
    float x = 0;
    float y = 0;
    vec2& operator+=(const vec2& l);
    float length() const;
};
std::iostream& operator>>(std::iostream& is, vec2& v);
struct vertex
{
    vertex()
        : pos()
        , uv()

    {
    }
    vertex(float x_, float y_, float tx_, float ty_)
        : pos(x_, y_)
        , uv(tx_, ty_)
    {
    }
    vec2 pos;
    vec2 uv;
};

struct triangle
{
    triangle()
    {
        v[0] = vertex();
        v[1] = vertex();
        v[2] = vertex();
    }
    triangle(vertex v0, vertex v1, vertex v2)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;
    }
    vertex v[3];
};
struct v2
{
    v2()
        : pos(0.0f, 0.0f)
        , uv(0.0f, 0.0f)
    {
    }
    v2(float x_pos, float y_pos, float x_uv, float y_uv)

    {
        pos.x = x_pos;
        pos.y = y_pos;
        uv.x  = x_uv;
        uv.y  = y_uv;
    }
    vec2 pos;
    vec2 uv;
};

struct tri2
{
    tri2();
    tri2(v2 v01, v2 v02, v2 v03);
    v2 v[3];
};
vec2 operator+(const vec2& l, const vec2& r);
vec2 operator-(const vec2& l, const vec2& r);

struct matrix
{
    matrix();
    static matrix identity();
    static matrix scale(float scale);
    static matrix scale(float sx, float sy);
    static matrix rotation(float thetha);
    static matrix move(const vec2& delta);
    vec2          row0;
    vec2          row1;
    vec2          row2;
};

vec2   operator*(const vec2& v, const matrix& m);
matrix operator*(const matrix& m1, const matrix& m2);

std::istream& operator>>(std::istream& is, v2& v);
std::istream& operator>>(std::istream& is, tri2& t);

std::istream& operator>>(std::istream& is, vertex& v);
std::istream& operator>>(std::istream& is, triangle& t);

class texture
{
public:
    virtual ~texture();
    virtual void          bind() const       = 0;
    virtual std::uint32_t get_width() const  = 0;
    virtual std::uint32_t get_height() const = 0;
};

class vertex_buffer
{
public:
    virtual ~vertex_buffer();
    virtual const v2* data() const = 0;
    /// count of vertexes
    virtual size_t size() const = 0;
};

class engine;

engine* create_engine();
void    destroy_engine(engine*);

class engine
{
public:
    ~engine();
    virtual std::string initialize(std::string_view)     = 0;
    virtual void        uninitialize()                   = 0;
    virtual bool        read_event(keys& key)            = 0;
    virtual bool        is_key_down(const enum keys key) = 0;
    virtual void        render_tet(const vertex_buffer& buff, texture* tex,
                                   const matrix& m)      = 0;
    // virtual bool check_input(const SDL_Event& e, const bind*& result) = 0;

    //    virtual index_buffer* create_index_buffer(const std::uint16_t*,
    //                                              std::size_t)    = 0;
    //    virtual void          destroy_index_buffer(index_buffer*) = 0;

    virtual vertex_buffer* create_vertex_buffer(const tri2* tri,
                                                std::size_t n) = 0;
    // virtual vertex_buffer* create_vertex_buffer(const v2*, std::size_t) = 0;
    virtual void destroy_vertex_buffer(vertex_buffer*) = 0;

    virtual texture* create_texture(std::string_view path)                  = 0;
    virtual texture* create_texture_part(std::string_view path,
                                         const size_t     width,
                                         const size_t     height)               = 0;
    virtual void     destroy_texture(texture* t)                            = 0;
    virtual void     render_triangle(triangle&)                             = 0;
    virtual void     render_text_triangle(triangle& t)                      = 0;
    virtual void     render_two_triangles(const std::vector<triangle>& v)   = 0;
    virtual void     render_tetris(const vertex_buffer& buff, texture* tex) = 0;
    virtual void     swap_buffer()                                          = 0;
    virtual bool     load_texture(std::string_view path, unsigned long w,
                                  unsigned long h)                          = 0;
};
