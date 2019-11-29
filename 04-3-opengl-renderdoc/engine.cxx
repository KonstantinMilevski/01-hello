#include "engine.hxx"

#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <vector>

#include "SDL.h"
#include "glad/glad.h"

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

std::ostream& operator<<(std::ostream& os, const SDL_version& v)
{
    os << v.major << ':';
    os << v.minor << ':';
    os << v.patch;
    return os;
}

std::istream& operator>>(std::istream& is, vertex& v)
{
    is >> v.x;
    is >> v.y;
    is >> v.z;

    is >> v.r;
    is >> v.g;
    is >> v.b;

    return is;
}
std::istream& operator>>(std::istream& is, triangle& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
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

        window = SDL_CreateWindow("window 04-3", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 640, 480,
                                  ::SDL_WINDOW_OPENGL);
        if (window == nullptr)
        {
            serr << "error: failed call SDL_CreateWindow: " << SDL_GetError()
                 << endl;
            return serr.str();
        }

        // RENDER_DOC//////// next code needed for RenderDoc to work
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                            SDL_GL_CONTEXT_PROFILE_CORE);
        // RENDER_DOC////////////////////////////////////////////////

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            std::string msg("can't create opengl context: ");
            msg += SDL_GetError();
            serr << msg << endl;
            return serr.str();
        }

        int gl_major_ver = 0;

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

        if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
        {
            std::cerr << "error: failed to initialize glad" << std::endl;
        }

        // RENDER_DOC///////////////////////////////////////////
        GLuint vertex_buffer = 0;
        glGenBuffers(1, &vertex_buffer);
        GL_CHECK()
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        GL_CHECK()
        GLuint vertex_array_object = 0;
        glGenVertexArrays(1, &vertex_array_object);
        GL_CHECK()
        glBindVertexArray(vertex_array_object);
        GL_CHECK()
        // RENDER_DOC///////////////////////////////////////////

        // create vertex shader
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        GL_CHECK();
        string_view vertex_source = R"(#version 330 core
                                    layout (location = 0) in vec3 a_position;
                                    layout (location = 1) in vec3 a_color;
                                    out vec4 v_position;
                                    out vec3 v_color;
                                    void main()
                                    {
                                        v_position = vec4(a_position, 1.0);
                                        v_color = a_color;
                                        gl_Position = v_position;
                                    }
                                    )";
        const char* source        = vertex_source.data();
        glShaderSource(vertex_shader, 1, &source, nullptr);
        GL_CHECK();

        glCompileShader(vertex_shader);
        GL_CHECK();

        GLint compiled_status = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled_status);
        GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &info_len);
            GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(vertex_shader, info_len, nullptr,
                               info_chars.data());
            GL_CHECK()
            glDeleteShader(vertex_shader);
            GL_CHECK()

            serr << "Error compiling shader(vertex)\n"
                 << vertex_source << "\n"
                 << info_chars.data();
            return serr.str();
        }

        // create fragment shader

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        GL_CHECK();
        string_view fragment_source = R"(#version 330 core
                                      precision mediump float;
                                      in vec4 v_position;
                                      in vec3 v_color;
                                      out vec4 FragColor;
                                      void main()
                                      {
                                          FragColor = vec4(v_color, 1.0);

                                          if (v_position.z >= 0.0)
                                          {
                                            float light_green = 0.5 + v_position.z / 2.0;
                                            FragColor = vec4(0.0, light_green, 0.0, 1.0);
                                          } else
                                          {
                                            float dark_green = 0.5 - (v_position.z / -2.0);
                                            FragColor = vec4(0.0, dark_green, 0.0, 1.0);
                                          }

                                      }
                                      )";
        source                      = fragment_source.data();
        glShaderSource(fragment_shader, 1, &source, nullptr);
        GL_CHECK();

        glCompileShader(fragment_shader);
        GL_CHECK();

        compiled_status = 0;
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled_status);
        GL_CHECK()
        if (compiled_status == 0)
        {
            GLint info_len = 0;
            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_len);
            GL_CHECK()
            std::vector<char> info_chars(static_cast<size_t>(info_len));
            glGetShaderInfoLog(fragment_shader, info_len, nullptr,
                               info_chars.data());
            GL_CHECK()
            glDeleteShader(fragment_shader);
            GL_CHECK()

            serr << "Error compiling shader(fragment)\n"
                 << fragment_source << "\n"
                 << info_chars.data();
            return serr.str();
        }

        // create program and attach vertex and fragment shaders

        program_id_ = glCreateProgram();
        GL_CHECK()
        if (0 == program_id_)
        {
            cerr << "error: can't create GL_program";
            return serr.str();
        }

        glAttachShader(program_id_, vertex_shader);
        GL_CHECK()
        glAttachShader(program_id_, fragment_shader);
        GL_CHECK()

        // bind attribute location
        glBindAttribLocation(program_id_, 0, "a_position");
        GL_CHECK()
        glBindAttribLocation(program_id_, 1, "a_color");
        GL_CHECK()

        glLinkProgram(program_id_);
        GL_CHECK()

        GLint linked_status = 0;
        glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
        GL_CHECK()
        if (linked_status == 0)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
            GL_CHECK()
            std::vector<char> infoLog(static_cast<size_t>(infoLen));
            glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
            GL_CHECK()
            serr << "Error linking program:\n" << infoLog.data();
            glDeleteProgram(program_id_);
            GL_CHECK()
            return serr.str();
        }
        glUseProgram(program_id_);
        GL_CHECK()

        glEnable(GL_DEPTH_TEST);
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
    void render_triangle(triangle& t) override final
    {

        // RENDER DOC addition ////////////////////
        glBufferData(GL_ARRAY_BUFFER, sizeof(t), &t, GL_STATIC_DRAW);
        GL_CHECK()
        glEnableVertexAttribArray(0);

        GLintptr position_attr_offset = 0;
        GL_CHECK()
        //

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                              reinterpret_cast<void*>(position_attr_offset));
        GL_CHECK()
        glEnableVertexAttribArray(1);
        GL_CHECK()
        //
        GLintptr color_attr_offset = sizeof(float) * 3;

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex),
                              reinterpret_cast<void*>(color_attr_offset));
        GL_CHECK()
        //

        glValidateProgram(program_id_);
        GL_CHECK()
        // Check the validate status
        GLint validate_status = 0;
        glGetProgramiv(program_id_, GL_VALIDATE_STATUS, &validate_status);
        GL_CHECK()
        if (validate_status == GL_FALSE)
        {
            GLint infoLen = 0;
            glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
            GL_CHECK()
            std::vector<char> infoLog(static_cast<size_t>(infoLen));
            glGetProgramInfoLog(program_id_, infoLen, nullptr, infoLog.data());
            GL_CHECK()
            std::cerr << "Error linking program:\n" << infoLog.data();
            throw std::runtime_error("error");
        }

        // glUniform4f(0, 0, 1, 1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        GL_CHECK()
    }
    void swap_buffer() override final
    {
        SDL_GL_SwapWindow(window);
        glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
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
            key = 1;

            if (sdl_event.type == SDL_QUIT)
            {
                key = 0;
                return true;
            }
        }

        return false;
    }

private:
    SDL_Window*   window      = nullptr;
    SDL_GLContext gl_context  = nullptr;
    GLuint        program_id_ = 0;
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
