#include "engine.hxx"
#include "color.hxx"
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

vec2::vec2()
    : x(0.f)
    , y(1.f)
{
}

vec2::vec2(float x_, float y_)
    : x(x_)
    , y(y_)
{
}

vec2 operator+(const vec2& l, const vec2& r)
{
    vec2 rez;
    rez.x = l.x + r.x;
    rez.y = l.y + r.y;
    return rez;
}

mat2x3::mat2x3()
    : col0(1.0f, 0.f)
    , col1(0.f, 1.f)
    , delta(0.f, 0.f)
{
}

mat2x3 mat2x3::identiry()
{
    return mat2x3::scale(1.f);
}
mat2x3 mat2x3::scale(float scale)
{
    mat2x3 result;
    result.col0.x = scale;
    result.col1.y = scale;
    return result;
}
mat2x3 mat2x3::scale(float sx, float sy)
{
    mat2x3 r;
    r.col0.x = sx;
    r.col1.y = sy;
    return r;
}

mat2x3 mat2x3::rotation(float thetha)
{
    mat2x3 result;

    result.col0.x = std::cos(thetha);
    result.col0.y = std::sin(thetha);

    result.col1.x = -std::sin(thetha);
    result.col1.y = std::cos(thetha);

    return result;
}

mat2x3 mat2x3::move(const vec2& delta)
{
    mat2x3 r = mat2x3::identiry();
    r.delta  = delta;
    return r;
}

vec2 operator*(const vec2& v, const mat2x3& m)
{
    vec2 result;
    result.x = v.x * m.col0.x + v.y * m.col0.y + m.delta.x;
    result.y = v.x * m.col1.x + v.y * m.col1.y + m.delta.y;
    return result;
}

mat2x3 operator*(const mat2x3& m1, const mat2x3& m2)
{
    mat2x3 r;

    r.col0.x = m1.col0.x * m2.col0.x + m1.col1.x * m2.col0.y;
    r.col1.x = m1.col0.x * m2.col1.x + m1.col1.x * m2.col1.y;
    r.col0.y = m1.col0.y * m2.col0.x + m1.col1.y * m2.col0.y;
    r.col1.y = m1.col0.y * m2.col1.x + m1.col1.y * m2.col1.y;

    r.delta.x = m1.delta.x * m2.col0.x + m1.delta.y * m2.col0.y + m2.delta.x;
    r.delta.y = m1.delta.x * m2.col1.x + m1.delta.y * m2.col1.y + m2.delta.y;

    return r;
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

std::istream& operator>>(std::istream& is, vec2& p)
{
    is >> p.x;
    is >> p.y;
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
vertex_buffer::~vertex_buffer() {}

class vertex_buffer_impl final : public vertex_buffer
{
public:
    vertex_buffer_impl(const tri2* tri, std::size_t n)
        : triangles(n)
    {
        assert(tri != nullptr);
        for (size_t i = 0; i < n; ++i)
        {
            triangles[i] = tri[i];
        }
    }
    ~vertex_buffer_impl() final;

    const v2*      data() const final { return &triangles.data()->v[0]; }
    virtual size_t size() const final { return triangles.size() * 3; }

private:
    std::vector<tri2> triangles;
};

vertex_buffer_impl::~vertex_buffer_impl() {}

static std::array<std::string_view, 17> event_names = {
    /// input events
    { "left_pressed", "left_released", "right_pressed", "right_released",
      "up_pressed", "up_released", "down_pressed", "down_released",
      "select_pressed", "select_released", "start_pressed", "start_released",
      "button1_pressed", "button1_released", "button2_pressed",
      "button2_released",
      /// virtual console events
      "turn_off" }
};
std::ostream& operator<<(std::ostream& stream, const event e)
{
    std::uint32_t value   = static_cast<std::uint32_t>(e);
    std::uint32_t minimal = static_cast<std::uint32_t>(event::left_pressed);
    std::uint32_t maximal = static_cast<std::uint32_t>(event::turn_off);
    if (value >= minimal && value <= maximal)
    {
        stream << event_names[value];
        return stream;
    }
    else
    {
        throw std::runtime_error("too big event value");
    }
}
struct bind
{
    bind(std::string_view s, SDL_Keycode k, event pressed, event released,
         keys om_k)
        : name(s)
        , key(k)
        , event_pressed(pressed)
        , event_released(released)
        , om_key(om_k)
    {
    }

    std::string_view name;
    SDL_Keycode      key;

    event event_pressed;
    event event_released;

    keys om_key;
};
const std::array<bind, 8> keys{
    { bind{ "up", SDLK_w, event::up_pressed, event::up_released, keys::up },
      bind{ "left", SDLK_a, event::left_pressed, event::left_released,
            keys::left },
      bind{ "down", SDLK_s, event::down_pressed, event::down_released,
            keys::down },
      bind{ "right", SDLK_d, event::right_pressed, event::right_released,
            keys::right },
      bind{ "button1", SDLK_LCTRL, event::button1_pressed,
            event::button1_released, keys::button1 },
      bind{ "button2", SDLK_SPACE, event::button2_pressed,
            event::button2_released, keys::button2 },
      bind{ "select", SDLK_ESCAPE, event::select_pressed,
            event::select_released, keys::select },
      bind{ "start", SDLK_RETURN, event::start_pressed, event::start_released,
            keys::start } }
};

static bool check_input(const SDL_Event& e, const bind*& result)
{
    using namespace std;

    const auto it = find_if(begin(keys), end(keys), [&](const ::bind& b) {
        return b.key == e.key.keysym.sym;
    });

    if (it != end(keys))
    {
        result = &(*it);
        return true;
    }
    return false;
}

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

        glGenBuffers(1, &gl_default_vbo);
        GL_CHECK()
        glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
        GL_CHECK()
        uint32_t data_size_in_bytes = 0;
        glBufferData(GL_ARRAY_BUFFER, data_size_in_bytes, nullptr,
                     GL_STATIC_DRAW);
        GL_CHECK()
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, nullptr);
        GL_CHECK()

        shader00 = new shader_gl_es20(R"(
                                  attribute vec2 a_position;
                                  void main()
                                  {
                                      gl_Position = vec4(a_position, 0.0, 1.0);
                                  }
                                  )",
                                      R"(
                                  #ifdef GL_ES
                                  precision mediump float;
                                  #endif
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
                #ifdef GL_ES
                precision mediump float;
                #endif
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
                v_tex_coord = a_tex_coord;
                v_color = a_color;
                gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
            R"(
                #ifdef GL_ES
                precision mediump float;
                #endif
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

        shader03 = new shader_gl_es20(
            R"(
                uniform mat3 u_matrix;
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                attribute vec4 a_color;
                varying vec4 v_color;
                varying vec2 v_tex_coord;
                void main()
                {
                v_tex_coord = a_tex_coord;
                v_color = a_color;
                vec3 pos = vec3(a_position, 1.0) * u_matrix;
                gl_Position = vec4(pos, 1.0);
                }
                )",
            R"(
                #ifdef GL_ES
                precision mediump float;
                #endif
                varying vec2 v_tex_coord;
                varying vec4 v_color;
                uniform sampler2D s_texture;
                void main()
                {
                gl_FragColor = texture2D(s_texture, v_tex_coord) * v_color;
                }
                )",
            { { 0, "a_position" }, { 1, "a_color" }, { 2, "a_tex_coord" } });
        shader03->use();

        glEnable(GL_BLEND);
        GL_CHECK()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GL_CHECK()
        glClearColor(0.f, 0.0, 0.f, 0.0f);
        GL_CHECK()

        return "";
    }
    void uninitialize() override final
    {
        // glDeleteProgram(program_id_);
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool read_event(event& e) override final
    {
        using namespace std;
        // collect all events from SDL
        SDL_Event sdl_event;
        if (SDL_PollEvent(&sdl_event))
        {
            const ::bind* binding = nullptr;

            if (sdl_event.type == SDL_QUIT)
            {
                e = event::turn_off;
                return true;
            }
            else if (sdl_event.type == SDL_KEYDOWN)
            {
                if (check_input(sdl_event, binding))
                {
                    e = binding->event_pressed;
                    return true;
                }
            }
            else if (sdl_event.type == SDL_KEYUP)
            {
                if (check_input(sdl_event, binding))
                {
                    e = binding->event_released;
                    return true;
                }
            }
        }
        return false;
    }

    bool is_key_down(const enum keys key) override final
    {
        const auto it =
            std::find_if(begin(keys), end(keys),
                         [&](const bind& b) { return b.om_key == key; });

        if (it != end(keys))
        {
            const std::uint8_t* state         = SDL_GetKeyboardState(nullptr);
            int                 sdl_scan_code = SDL_GetScancodeFromKey(it->key);
            return state[sdl_scan_code];
        }
        return false;
    }

    texture* create_texture(std::string_view path) final
    {
        return new texture_gl_es20(path);
    }
    void destroy_texture(texture* t) final { delete t; }

    vertex_buffer* create_vertex_buffer(const tri2* triangles, std::size_t n)
    {
        return new vertex_buffer_impl(triangles, n);
    }
    void destroy_vertex_buffer(vertex_buffer* buffer) { delete buffer; }

    void render(const tri0& t, const color& c) final
    {
        shader00->use();
        shader00->set_uniform("u_color", c);
        // vertex coordinates
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v0),
                              &t.v[0].p.x);
        GL_CHECK()
        glEnableVertexAttribArray(0);
        GL_CHECK()

        // texture coordinates
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(v0),
        // &t.v[0].tx);
        // GL_CHECK()
        // glEnableVertexAttribArray(1);
        // GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK()
    }
    void render(const tri1& t) final
    {
        shader01->use();
        // positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].p);
        GL_CHECK()
        glEnableVertexAttribArray(0);
        GL_CHECK()
        // colors
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                              &t.v[0].c);
        GL_CHECK()
        glEnableVertexAttribArray(1);
        GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK()

        glDisableVertexAttribArray(1);
        GL_CHECK()
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
        GL_CHECK()
        glEnableVertexAttribArray(0);
        GL_CHECK()
        // colors
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                              &t.v[0].c);
        GL_CHECK()
        glEnableVertexAttribArray(1);
        GL_CHECK()

        // texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].uv);
        GL_CHECK()
        glEnableVertexAttribArray(2);
        GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK()

        glDisableVertexAttribArray(1);
        GL_CHECK()
        glDisableVertexAttribArray(2);
        GL_CHECK()
    }
    void render(const tri2& t, texture* tex, const mat2x3& m)
    {
        shader03->use();
        texture_gl_es20* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader03->set_uniform("s_texture", texture);
        shader03->set_uniform("u_matrix", m);
        // positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].p);
        GL_CHECK()
        glEnableVertexAttribArray(0);
        GL_CHECK()
        // colors
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t.v[0]),
                              &t.v[0].c);
        GL_CHECK()
        glEnableVertexAttribArray(1);
        GL_CHECK()

        // texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(t.v[0]),
                              &t.v[0].uv);
        GL_CHECK()
        glEnableVertexAttribArray(2);
        GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK()

        glDisableVertexAttribArray(1);
        GL_CHECK()
        glDisableVertexAttribArray(2);
        GL_CHECK()
    }

    void render(const vertex_buffer& buff, texture* tex, const mat2x3& m) final
    {
        shader03->use();
        texture_gl_es20* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader03->set_uniform("s_texture", texture);
        shader03->set_uniform("u_matrix", m);

        assert(gl_default_vbo != 0);

        glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
        GL_CHECK()

        const v2* t = buff.data();
        uint32_t  data_size_in_bytes =
            static_cast<uint32_t>(buff.size() * sizeof(v2));
        /// create and initialize a buffer object's data store
        glBufferData(
            GL_ARRAY_BUFFER, data_size_in_bytes,
            t, /// Specifies a pointer to data that will be copied into the data
               /// store for initialization, or NULL if no data is to be copied.
            GL_DYNAMIC_DRAW /// Specifies the expected usage pattern of the data
                            /// store
        );
        GL_CHECK()
        /// update a subset of a buffer object's data store
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, t);
        GL_CHECK()

        // positions
        glEnableVertexAttribArray(0);
        GL_CHECK()
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), nullptr);
        GL_CHECK()
        // colors
        glVertexAttribPointer(
            1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(v2),
            reinterpret_cast<void*>(sizeof(v2::p) + sizeof(v2::uv)));
        GL_CHECK()
        glEnableVertexAttribArray(1);
        GL_CHECK()

        // texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(v2),
                              reinterpret_cast<void*>(sizeof(v2::p)));
        GL_CHECK()
        glEnableVertexAttribArray(2);
        GL_CHECK()

        GLsizei num_of_vertexes = static_cast<GLsizei>(buff.size());
        glDrawArrays(GL_TRIANGLES, 0, num_of_vertexes);
        GL_CHECK()

        glDisableVertexAttribArray(1);
        GL_CHECK()
        glDisableVertexAttribArray(2);
        GL_CHECK()
    }
    void swap_buffer() override final
    {
        SDL_GL_SwapWindow(window);
        // glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
        GL_CHECK()
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK()
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

    shader_gl_es20* shader00       = nullptr;
    shader_gl_es20* shader01       = nullptr;
    shader_gl_es20* shader02       = nullptr;
    shader_gl_es20* shader03       = nullptr;
    uint32_t        gl_default_vbo = 0;
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
