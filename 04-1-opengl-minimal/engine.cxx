#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>

#include "SDL.h"
#include "engine.hxx"
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

        //        SDL_version compiled = { 0, 0, 0 };
        //        SDL_version linked   = { 0, 0, 0 };

        //        SDL_VERSION(&compiled);
        //        SDL_GetVersion(&linked);

        //        if (SDL_COMPILEDVERSION !=
        //            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        //        {
        //            serr << "warning: SDL2 compiled and linked version
        //            mismatch: "
        //                 << compiled << " " << linked << endl;
        //        }

        const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
        //        if (0 != init_result)
        //        {
        //            const char* err_message = SDL_GetError();
        //            serr << "error: failed call SDL_Init: " << err_message <<
        //            endl; return serr.str();
        //        }

        window = SDL_CreateWindow("window 04-01", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, 640, 480,
                                  ::SDL_WINDOW_OPENGL);
        //        if (window == nullptr)
        //        {
        //            const char* err_message = SDL_GetError();
        //            serr << "error: failed call SDL_CreateWindow: " <<
        //            err_message
        //                 << endl;
        //            SDL_Quit();
        //            return serr.str();
        //        }

        //        int gl_major_ver = 2;
        //        int gl_minor_ver = 0;

        //        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
        //                            SDL_GL_CONTEXT_PROFILE_ES);
        //        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
        //        gl_major_ver);
        //        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
        //        gl_minor_ver);

        gl_context = SDL_GL_CreateContext(window);
        // check gl_context by NULL
        gladLoadGLES2Loader(SDL_GL_GetProcAddress);
        // check func by NULL
        return "";
    }
    void uninitialize() override final
    {

        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    void render_triangle(triangle& t) override final
    {
        glClearColor(0.f, 1.0, 1.f, 0.0f);
        GL_CHECK();
        glClear(GL_COLOR_BUFFER_BIT);
        GL_CHECK();
    }
    void swap_buffer() override final { SDL_GL_SwapWindow(window); }

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
        else
            return false;
    }

private:
    SDL_Window*   window     = nullptr;
    SDL_GLContext gl_context = nullptr;
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
