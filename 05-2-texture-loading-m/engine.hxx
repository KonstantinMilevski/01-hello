#pragma once

#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

struct vertex
{
    vertex()
        : x(0.0f)
        , y(0.0f)
        , tx(0.0f)
        , ty(0.0f)
    {
    }
    float x;
    float y;

    float tx = 0.f; // texture coordinate
    float ty = 0.f;
};

struct triangle
{
    triangle()
    {
        v[0] = vertex();
        v[1] = vertex();
        v[2] = vertex();
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
    virtual std::string initialize(std::string_view)     = 0;
    virtual void        uninitialize()                   = 0;
    virtual void        render_triangle(const triangle&) = 0;
    virtual void        swap_buffer()                    = 0;
    /// execute while "close SDLevent"
    virtual bool read_input(bool&) = 0;
    /// return seconds from initialization
    virtual float get_time_from_init()                = 0;
    virtual bool  load_texture(std::string_view path) = 0;
};
