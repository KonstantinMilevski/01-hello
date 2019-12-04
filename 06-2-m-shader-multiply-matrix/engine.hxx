#pragma once

#include "color.hxx"
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>
////////////////////////////////////////////////////////////

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
    static mat2x3 rotation(float thetha);
    static mat2x3 move(const vec2& delta);
    vec2          col0;
    vec2          col1;
    vec2          delta;
};

vec2   operator*(const vec2& v, const mat2x3& m);
mat2x3 operator*(const mat2x3& m1, const mat2x3& m2);

// struct mat2
//{
//    mat2();
//    static mat2 identiry();
//    static mat2 scale(float scale);
//    static mat2 rotate(float alfa);
//    vec2        col0;
//    vec2        col1;
//};
// vec2 operator*(const vec2& v, const mat2& m);
// mat2 operator*(const mat2& m1, const mat2& m2);

///////////////////////////////////////////////////////////

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

// std::istream& operator>>(std::istream& is, mat2&);
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
    virtual float    get_time_from_init()                           = 0;
    virtual texture* create_texture(std::string_view path)          = 0;
    virtual void     destroy_texture(texture*)                      = 0;
    virtual void     render(const tri0&, const color&)              = 0;
    virtual void     render(const tri1&)                            = 0;
    virtual void     render(const tri2&, texture*)                  = 0;
    virtual void     render(const tri2&, texture*, const mat2x3& m) = 0;
};
