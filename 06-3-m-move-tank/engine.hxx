#pragma once
#include "SDL2/SDL.h"

#include "color.hxx"
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

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

enum class keys
{
    left,
    right,
    up,
    down,
    select,
    start,
    button1,
    button2
};

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

std::ostream& operator<<(std::ostream& stream, const event e);

struct vec2
{
    vec2();
    vec2(float x_, float y_);
    float x = 0;
    float y = 0;
};
vec2 operator+(const vec2& l, const vec2& r);

struct mat2x3
{
    mat2x3();
    static mat2x3 identiry();
    static mat2x3 scale(float scale);
    static mat2x3 scale(float sx, float sy);
    static mat2x3 rotation(float thetha);
    static mat2x3 move(const vec2& delta);
    vec2          col0;
    vec2          col1;
    vec2          delta;
};

vec2   operator*(const vec2& v, const mat2x3& m);
mat2x3 operator*(const mat2x3& m1, const mat2x3& m2);

struct v0
{
    vec2 p;
};
struct v1
{
    vec2  p;
    color c;
};
struct v2
{
    vec2  p;
    vec2  uv;
    color c;
};

struct tri0
{
    tri0();
    v0 v[3];
};
struct tri1
{
    tri1();
    v1 v[3];
};
struct tri2
{
    tri2();
    v2 v[3];
};

std::istream& operator>>(std::istream&, vec2&);
std::istream& operator>>(std::istream&, v0&);
std::istream& operator>>(std::istream&, v1&);
std::istream& operator>>(std::istream&, v2&);
std::istream& operator>>(std::istream&, tri0&);
std::istream& operator>>(std::istream&, tri1&);
std::istream& operator>>(std::istream&, tri2&);

class engine;
engine* create_engine();
void    destroy_engine(engine*);

class texture
{
public:
    virtual ~texture();
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

class engine
{
public:
    virtual ~engine();
    /// create main window
    /// on success return empty string
    virtual std::string initialize(std::string_view) = 0;
    virtual void        swap_buffer()                = 0;
    virtual void        uninitialize()               = 0;

    /// pool event from input queue
    virtual bool read_event(event& e)    = 0;
    virtual bool is_key_down(const keys) = 0;

    /// return seconds from initialization
    virtual float get_time_from_init() = 0;

    virtual texture* create_texture(std::string_view path) = 0;
    virtual void     destroy_texture(texture*)             = 0;

    virtual vertex_buffer* create_vertex_buffer(const tri2*, std::size_t) = 0;
    virtual void           destroy_vertex_buffer(vertex_buffer*)          = 0;

    virtual void render(const tri0&, const color&)                     = 0;
    virtual void render(const tri1&)                                   = 0;
    virtual void render(const tri2&, texture*)                         = 0;
    virtual void render(const tri2&, texture*, const mat2x3& m)        = 0;
    virtual void render(const vertex_buffer&, texture*, const mat2x3&) = 0;
};
