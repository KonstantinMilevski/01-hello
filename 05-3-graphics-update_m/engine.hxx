#pragma once

#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

class color
{
public:
    color() = default;
    explicit color(std::uint32_t rgba_);
    color(float r, float g, float b, float a);

    float get_r() const;
    float get_g() const;
    float get_b() const;
    float get_a() const;

    void set_r(const float r);
    void set_g(const float g);
    void set_b(const float b);
    void set_a(const float a);

private:
    std::uint32_t rgba = 0;
};

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
