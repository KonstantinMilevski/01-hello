#pragma once

#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

#include "picopng.hxx"

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
    pause
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

class engine;

engine* create_engine();
void    destroy_engine(engine*);

class engine
{
public:
    ~engine();
    virtual std::string initialize(std::string_view) = 0;
    virtual void        uninitialize()               = 0;
    virtual bool        read_event(event& e)         = 0;
    virtual bool        read_input(int&)             = 0;

    virtual void render_triangle(triangle&)        = 0;
    virtual void render_text_triangle(triangle& t) = 0;
    virtual void swap_buffer()                     = 0;
    virtual bool load_texture(std::string_view path, unsigned long w,
                              unsigned long h)     = 0;
};
