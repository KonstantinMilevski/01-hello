#pragma once

#include "SDL2/SDL.h"
#include "color.hxx"
#include "glad/glad.h"

#include <iostream>
#include <sstream>
#include <string_view>

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

struct pos
{
    float x = 0.0f;
    float y = 0.0f;
};
struct uv_pos
{
    float u = 0.0f;
    float v = 0.0f;
};

struct v0
{
    pos p;
};
struct v1
{
    pos   p;
    color c;
};
struct v2
{
    pos    p;
    uv_pos uv;
    color  c;
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

std::istream& operator>>(std::istream&, pos&);
std::istream& operator>>(std::istream&, uv_pos&);
std::istream& operator>>(std::istream&, color&);
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

class engine
{
public:
    ~engine();
    /// create main window
    /// on success return empty string
    virtual std::string initialize(std::string_view) = 0;
    virtual void        uninitialize()               = 0;
    virtual void        swap_buffer()                = 0;
    virtual bool        read_input(int&)             = 0;
    /// return seconds from initialization
    virtual float    get_time_from_init()                  = 0;
    virtual texture* create_texture(std::string_view path) = 0;
    virtual void     destroy_texture(texture*)             = 0;
    virtual void     render(const tri0&, const color&)     = 0;
    virtual void     render(const tri1&)                   = 0;
    virtual void     render(const tri2&, texture*)         = 0;
};
