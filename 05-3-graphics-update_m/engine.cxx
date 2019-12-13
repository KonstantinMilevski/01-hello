#include "engine.hxx"

#include "color.hxx"
#include "glad/glad.h"
#include "shader_gl_es20.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <vector>

std::ostream& operator<<(std::ostream& os, const SDL_version& v)
{
    os << v.major << ':';
    os << v.minor << ':';
    os << v.patch;
    return os;
}

tri0::tri0()
    : v{ v0(), v0(), v0() }
{
}

tri1::tri1()
    : v{ v1(), v1(), v1() }
{
}

tri2::tri2()
    : v{ v2(), v2(), v2() }
{
}

std::istream& operator>>(std::istream& is, pos& p)
{
    is >> p.x;
    is >> p.y;
    return is;
}
std::istream& operator>>(std::istream& is, uv_pos& uv)
{
    is >> uv.u;
    is >> uv.v;
    return is;
}
std::istream& operator>>(std::istream& is, color& c)
{
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    float a = 0.f;
    is >> r;
    is >> g;
    is >> b;
    is >> a;
    c = color(r, g, b, a);
    return is;
}
std::istream& operator>>(std::istream& is, v0& v)
{
    is >> v.p;
    return is;
}
std::istream& operator>>(std::istream& is, v1& v)
{
    is >> v.p;
    is >> v.c;
    return is;
}
std::istream& operator>>(std::istream& is, v2& v)
{
    is >> v.p;
    is >> v.uv;
    is >> v.c;
    return is;
}
std::istream& operator>>(std::istream& is, tri0& tr)
{
    is >> tr.v[0];
    is >> tr.v[1];
    is >> tr.v[2];
    return is;
}

std::istream& operator>>(std::istream& is, tri1& tr)
{
    is >> tr.v[0];
    is >> tr.v[1];
    is >> tr.v[2];
    return is;
}
std::istream& operator>>(std::istream& is, tri2& tr)
{
    is >> tr.v[0];
    is >> tr.v[1];
    is >> tr.v[2];
    return is;
}
/////////////////////
texture::~texture() {}

class engine_impl final : public engine
{
public:
    std::string initialize(std::string_view str) override final
    {

        using namespace std;

        stringstream serr;

        SDL_version compiled = { 0, 0, 0 };
        SDL_version linked   = { 0, 0, 0 };
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        if (SDL_COMPILEDVERSION !=
            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        {
            serr << "warning: SDL2 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            serr << "error: failed call SDL_Init: " << SDL_GetError() << endl;
            return serr.str();
        }

        window = SDL_CreateWindow("window 05-3", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 640, 480,
                                  ::SDL_WINDOW_OPENGL);
        if (window == nullptr)
        {
            serr << "error: failed call SDL_CreateWindow: " << SDL_GetError()
                 << endl;
            return serr.str();
        }

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            std::string msg("can't create opengl context: ");
            msg += SDL_GetError();
            serr << msg << endl;
            return serr.str();
        }

        int gl_major_ver = 2;

        int result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
        SDL_assert(result == 0);
        int gl_minor_ver = 0;
        result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
        SDL_assert(result == 0);

        if (gl_major_ver <= 2 && gl_minor_ver < 1)
        {
            serr << "current context opengl version: " << gl_major_ver << '.'
                 << gl_minor_ver << '\n'
                 << "need opengl version at least: 2.1\n"
                 << std::flush;
            return serr.str();
        }

        if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
        {
            std::cerr << "error: failed to initialize glad" << std::endl;
        }

        shader00 = new shader_gl_es20(R"(
                                  attribute vec2 a_position;
                                  void main()
                                  {
                                      gl_Position = vec4(a_position, 0.0, 1.0);
                                  }
                                  )",
                                      R"(
                                  uniform vec4 u_color;
                                  void main()
                                  {
                                      gl_FragColor = u_color;
                                  }
                                  )",
                                      { { 0, "a_position" } });

        shader00->use();
        shader00->set_uniform("u_color", color(1.f, 0.f, 0.f, 1.f));

        shader01 = new shader_gl_es20(
            R"(
                attribute vec2 a_position;
                attribute vec4 a_color;
                varying vec4 v_color;
                void main()
                {
                    v_color = a_color;
                    gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
            R"(
                varying vec4 v_color;
                void main()
                {
                     gl_FragColor = v_color;
                }
                )",
            { { 0, "a_position" }, { 1, "a_color" } });

        shader01->use();

        shader02 = new shader_gl_es20(
            R"(
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                attribute vec4 a_color;
                varying vec4 v_color;
                varying vec2 v_tex_coord;
                void main()
                {
                    v_tex_coord=vec2(a_tex_coord.x*2.0f, a_tex_coord.y*2.0f);
                    v_color = a_color;
                    gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
            R"(
                varying vec2 v_tex_coord;
                varying vec4 v_color;
                uniform sampler2D s_texture;
                void main()
                {
                     gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                }
                )",
            { { 0, "a_position" }, { 1, "a_color" }, { 2, "a_tex_coord" } });

        // turn on rendering with just created shader program
        shader02->use();

        glEnable(GL_BLEND);
        GL_CHECK();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GL_CHECK();
        glClearColor(0.f, 0.0, 0.f, 0.0f);
        GL_CHECK();

        return "";
    }
    void uninitialize() override final
    {
        // glDeleteProgram(program_id_);
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    /*----------------------------------------------------------------------------*/
    texture* create_texture(std::string_view path) final
    {
        return new texture_gl_es20(path);
    }
    void destroy_texture(texture* t) final { delete t; }

    void render(const tri0& t, const color& c) final
    {
        shader00->use();
        shader00->set_uniform("u_color", c);
        // vertex coordinates

        glVertexAttribPointer(0, // 0 - position attribute
                              2, GL_FLOAT,
                              GL_FALSE, // float==float (без приведания)
                              sizeof(v0), &t.v[0].p.x);
        GL_CHECK();
        glEnableVertexAttribArray(0);
        GL_CHECK();

        // texture coordinates
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(v0),
        // &t.v[0].tx);
        // GL_CHECK();
        // glEnableVertexAttribArray(1);
        // GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK();
    }
    void render(const tri1& t) final
    {
        shader01->use();
        // positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].p);
        GL_CHECK();
        glEnableVertexAttribArray(0);
        GL_CHECK();
        // colors
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                              &t.v[0].c);
        GL_CHECK();
        glEnableVertexAttribArray(1);
        GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK();

        glDisableVertexAttribArray(1);
        GL_CHECK();
    }
    void render(const tri2& t, texture* tex) final
    {
        shader02->use();
        texture_gl_es20* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader02->set_uniform("s_texture", texture);
        // positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].p);
        GL_CHECK();
        glEnableVertexAttribArray(0);
        GL_CHECK();
        // colors
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                              &t.v[0].c);
        GL_CHECK();
        glEnableVertexAttribArray(1);
        GL_CHECK();

        // texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].uv);
        GL_CHECK();
        glEnableVertexAttribArray(2);
        GL_CHECK();

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK();

        glDisableVertexAttribArray(1);
        GL_CHECK();
        glDisableVertexAttribArray(2);
        GL_CHECK();
    }
    void swap_buffer() override final
    {
        SDL_GL_SwapWindow(window);
        // glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
        GL_CHECK()
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK()
    }

    bool read_input(int& key) override final
    {
        using namespace std;
        SDL_Event sdl_event;
        if (SDL_PollEvent(&sdl_event))
        {
            if (sdl_event.type == SDL_QUIT)
            {
                key = -1;
                return true;
            }
            if (sdl_event.key.keysym.sym == SDLK_SPACE)
            {
                key = 1;
                return true;
            }
        }

        return false;
    }

    float get_time_from_init() override final
    {
        std::uint32_t time_from_begining = SDL_GetTicks();
        float         current_time       = time_from_begining * 0.001f;
        return current_time;
    }

private:
    SDL_Window*   window     = nullptr;
    SDL_GLContext gl_context = nullptr;

    shader_gl_es20* shader00 = nullptr;
    shader_gl_es20* shader01 = nullptr;
    shader_gl_es20* shader02 = nullptr;
};
engine::~engine() {}

static bool already_exist = false;

engine* create_engine()
{
    if (true == already_exist)
    {
        throw std::runtime_error("engine allredy exist");
    }
    engine* eng   = new engine_impl();
    already_exist = true;
    return eng;
}
void destroy_engine(engine* eng)
{
    if (false == already_exist)
    {
        throw std::runtime_error("engine no exist");
    }
    if (nullptr == eng)
    {
        throw std::runtime_error("engine is null");
    }
    delete eng;
}
