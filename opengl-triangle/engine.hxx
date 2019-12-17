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
};

struct vertex
{
    vertex()
        : x(0.0f)
        , y(0.0f)
        , tx(0.0f)
        , ty(0.0f)
    {
    }
    vertex(float x_, float y_, float tx_, float ty_)
        : x(x_)
        , y(y_)
        , tx(tx_)
        , ty(ty_)
    {
    }
    float x;
    float y;
    float tx;
    float ty;
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
    virtual void          bind() const = 0;
    virtual std::uint32_t size() const = 0;
};

class index_buffer
{
public:
    virtual ~index_buffer();
    virtual void          bind() const = 0;
    virtual std::uint32_t size() const = 0;
};

class engine;

engine* create_engine();
void    destroy_engine(engine*);

class engine
{
public:
    ~engine();
    virtual std::string initialize(std::string_view) = 0;
    virtual void        uninitialize()               = 0;
    virtual bool        read_event(keys& key)        = 0;
    // virtual bool        check_input(keys&)           = 0;

    //    virtual index_buffer* create_index_buffer(const std::uint16_t*,
    //                                              std::size_t)    = 0;
    //    virtual void          destroy_index_buffer(index_buffer*) = 0;

    virtual texture* create_texture(std::string_view path) = 0;
    //    virtual texture* create_texture_rgba32(const void*  pixels,
    //                                           const size_t width,
    //                                           const size_t height) = 0;
    virtual void destroy_texture(texture* t)                          = 0;
    virtual void render_triangle(triangle&)                           = 0;
    virtual void render_text_triangle(triangle& t)                    = 0;
    virtual void render_two_triangles(const std::vector<triangle>& v) = 0;
    virtual void swap_buffer()                                        = 0;
    virtual bool load_texture(std::string_view path, unsigned long w,
                              unsigned long h)                        = 0;
};
