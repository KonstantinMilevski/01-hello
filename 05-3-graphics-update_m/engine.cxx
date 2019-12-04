#include "engine.hxx"
#include "SDL.h"
#include "glad/glad.h"
#include "picopng.hxx"

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

#pragma pack(push, 1)
/*---------------------------------------------------------------------*/
class texture_gl_es20 final : public texture
{
public:
    /// read png and create texture
    explicit texture_gl_es20(std::string_view path);
    ~texture_gl_es20() override;

    void bind() const
    {
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        GL_CHECK();
    }

    std::uint32_t get_width() const final { return width; }
    std::uint32_t get_height() const final { return height; }

private:
    std::string file_path;
    /// индентификатор текстуры
    GLuint        tex_handl = 0;
    std::uint32_t width     = 0;
    std::uint32_t height    = 0;
};
#pragma pack(pop)
/*-----------------------------------------------------------------------*/
class shader_gl_es20
{
public:
    shader_gl_es20(
        std::string_view vertex_src, std::string_view fragment_src,
        const std::vector<std::tuple<GLuint, const GLchar*>>& attributes)
    {
        vert_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
        frag_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
        if (vert_shader == 0 || frag_shader == 0)
        {
            throw std::runtime_error("can't compile shader");
        }
        program_id = link_shader_program(attributes);
        if (program_id == 0)
        {
            throw std::runtime_error("can't link shader");
        }
    }

    void use() const
    {
        glUseProgram(program_id);
        GL_CHECK();
    }

    void set_uniform(std::string_view uniform_name, texture_gl_es20* texture)
    {
        assert(texture != nullptr);
        const int location =
            glGetUniformLocation(program_id, uniform_name.data());
        GL_CHECK();
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        unsigned int texture_unit = 0;
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        GL_CHECK();

        texture->bind();

        // http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
        glUniform1i(location, static_cast<int>(0 + texture_unit));
        GL_CHECK();
    }

    void set_uniform(std::string_view uniform_name, const color& c)
    {
        const int location =
            glGetUniformLocation(program_id, uniform_name.data());
        GL_CHECK();
        if (location == -1)
        {
            std::cerr << "can't get uniform location from shader\n";
            throw std::runtime_error("can't get uniform location");
        }
        float values[4] = { c.get_r(), c.get_g(), c.get_b(), c.get_a() };
        glUniform4fv(location, 1, &values[0]);
        GL_CHECK();
    }

private:
    GLuint compile_shader(GLenum shader_type, std::string_view src)
    {
        GLuint shader_id = glCreateShader(shader_type);
        GL_CHECK();
        std::string_view vertex_shader_src = src;
        const char*      source            = vertex_shader_src.data();
        glShaderSource(shader_id, 1, &source, nullptr);
        GL_CHECK();

        glCompileShader(shader_id);
        GL_CHECK();

        GLint compiled_status = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_status);
        GL_CHECK();
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_len);
            GL_CHECK();
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(shader_id, info_len, nullptr, info_chars.data());
            GL_CHECK();
            glDeleteShader(shader_id);
            GL_CHECK();

            std::string shader_type_name =
                shader_type == GL_VERTEX_SHADER ? "vertex" : "fragment";
            std::cerr << "Error compiling shader(vertex)\n"
                      << vertex_shader_src << "\n"
                      << info_chars.data();
            return 0;
        }
        return shader_id;
    }
    /// create prog, attach shader, bind attribute, link prog
    GLuint link_shader_program(
        const std::vector<std::tuple<GLuint, const GLchar*>>& attributes)
    {
        GLuint program_id_ = glCreateProgram();
        GL_CHECK();
        if (0 == program_id_)
        {
            std::cerr << "failed to create gl program";
            throw std::runtime_error("can't link shader");
        }

        glAttachShader(program_id_, vert_shader);
        GL_CHECK();
        glAttachShader(program_id_, frag_shader);
        GL_CHECK();

        // bind attribute location
        for (const auto& attr : attributes)
        {
            GLuint        loc  = std::get<0>(attr);
            const GLchar* name = std::get<1>(attr);
            glBindAttribLocation(program_id_, loc, name);
            GL_CHECK();
        }

        // link program after binding attribute locations
        glLinkProgram(program_id_);
        GL_CHECK();
        // Check the link status
        GLint linked_status = 0;
        glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
        GL_CHECK();
        if (linked_status == 0)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
            GL_CHECK();
            std::vector<char> infoLog(static_cast<size_t>(infoLen));
            glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
            GL_CHECK();
            std::cerr << "Error linking program:\n" << infoLog.data();
            glDeleteProgram(program_id_);
            GL_CHECK();
            return 0;
        }
        return program_id_;
    }

    GLuint vert_shader = 0;
    GLuint frag_shader = 0;
    GLuint program_id  = 0;
};
/*----------------------------------------------------------------------------------------*/

//////////////////////////////
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

        window = SDL_CreateWindow("window 05-2", SDL_WINDOWPOS_CENTERED,
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
                    v_tex_coord = a_tex_coord;
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
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v0),
                              &t.v[0].p.x);
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
            if (sdl_event.type == SDLK_SPACE)
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

texture_gl_es20::texture_gl_es20(std::string_view path)
    : file_path(path)
{
    std::vector<unsigned char> png_file_in_memory;
    std::ifstream              ifs(path.data(), std::ios_base::binary);
    if (!ifs)
    {
        throw std::runtime_error("can't load texture");
    }
    ifs.seekg(0, std::ios_base::end);
    std::streamoff pos_in_file = ifs.tellg();
    png_file_in_memory.resize(static_cast<size_t>(pos_in_file));
    ifs.seekg(0, std::ios_base::beg);
    if (!ifs)
    {
        throw std::runtime_error("can't load texture");
    }

    ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()), pos_in_file);
    if (!ifs.good())
    {
        throw std::runtime_error("can't load texture");
    }

    std::vector<unsigned char> image;
    unsigned long              w = 0;
    unsigned long              h = 0;
    int error = decodePNG(image, w, h, &png_file_in_memory[0],
                          png_file_in_memory.size(), false);

    // if there's an error, display it
    if (error != 0)
    {
        std::cerr << "error: " << error << std::endl;
        throw std::runtime_error("can't load texture");
    }

    //Первый указывает количество имен-идентификаторов текстур,
    //которые нужно создать. Второй параметр - указатель на массив элементов
    //типа GLuint.
    glGenTextures(1, &tex_handl);
    GL_CHECK();
    //привязываемся к текстуре фотографии, т.е. делаем ее активной.
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    GL_CHECK();

    GLint   mipmap_level = 0;
    GLint   border       = 0;
    GLsizei width_       = static_cast<GLsizei>(w);
    GLsizei height_      = static_cast<GLsizei>(h);
    //создать саму текстуру в памяти
    glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, width_, height_, border,
                 GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    GL_CHECK();
    //установить параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK();
}

texture_gl_es20::~texture_gl_es20()
{
    glDeleteTextures(1, &tex_handl);
    GL_CHECK();
}

color::color(std::uint32_t rgba_)
    : rgba(rgba_)
{
}
color::color(float r, float g, float b, float a)
{
    assert(r <= 1 && r >= 0);
    assert(g <= 1 && g >= 0);
    assert(b <= 1 && b >= 0);
    assert(a <= 1 && a >= 0);

    std::uint32_t r_ = static_cast<std::uint32_t>(r * 255);
    std::uint32_t g_ = static_cast<std::uint32_t>(g * 255);
    std::uint32_t b_ = static_cast<std::uint32_t>(b * 255);
    std::uint32_t a_ = static_cast<std::uint32_t>(a * 255);

    rgba = a_ << 24 | b_ << 16 | g_ << 8 | r_;
}

float color::get_r() const
{
    std::uint32_t r_ = (rgba & 0x000000FF) >> 0;
    return r_ / 255.f;
}
float color::get_g() const
{
    std::uint32_t g_ = (rgba & 0x0000FF00) >> 8;
    return g_ / 255.f;
}
float color::get_b() const
{
    std::uint32_t b_ = (rgba & 0x00FF0000) >> 16;
    return b_ / 255.f;
}
float color::get_a() const
{
    std::uint32_t a_ = (rgba & 0xFF000000) >> 24;
    return a_ / 255.f;
}

void color::set_r(const float r)
{
    std::uint32_t r_ = static_cast<std::uint32_t>(r * 255);
    rgba &= 0xFFFFFF00;
    rgba |= (r_ << 0);
}
void color::set_g(const float g)
{
    std::uint32_t g_ = static_cast<std::uint32_t>(g * 255);
    rgba &= 0xFFFF00FF;
    rgba |= (g_ << 8);
}
void color::set_b(const float b)
{
    std::uint32_t b_ = static_cast<std::uint32_t>(b * 255);
    rgba &= 0xFF00FFFF;
    rgba |= (b_ << 16);
}
void color::set_a(const float a)
{
    std::uint32_t a_ = static_cast<std::uint32_t>(a * 255);
    rgba &= 0x00FFFFFF;
    rgba |= a_ << 24;
}
