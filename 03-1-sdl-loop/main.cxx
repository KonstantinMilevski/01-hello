#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <string_view>

#include <SDL.h>
std::ostream& operator<<(std::ostream& out, SDL_version& v)
{
    out << static_cast<int>(v.major) << " ";
    out << static_cast<int>(v.minor) << " ";
    out << static_cast<int>(v.patch);
    return out;
}
struct bind
{
    SDL_Keycode      key;
    std::string_view name;
};

#pragma pack(push, 4)
void check_input(SDL_Event& e)
{
    using namespace std;
    const array<::bind, 8> keys{ { { SDLK_w, "up" },
                                   { SDLK_a, "left" },
                                   { SDLK_s, "down" },
                                   { SDLK_d, "right" },
                                   { SDLK_LCTRL, "button_one" },
                                   { SDLK_SPACE, "button_two" },
                                   { SDLK_ESCAPE, "select" },
                                   { SDLK_RETURN, "start" } } };
    const auto it = find_if(begin(keys), end(keys), [&](const ::bind& b) {
        return b.key == e.key.keysym.sym;
    });
    if (it != end(keys))
        cout << it->name << ' ';
    if (e.type == SDL_KEYUP)
        cout << "is pressed" << endl;
    else
        cout << "is released" << endl;
}
#pragma pack(pop)

int main()

{
    using namespace std;

    SDL_version compiled = { 0, 0, 0 };
    SDL_version linked   = { 0, 0, 0 };

    SDL_VERSION(&compiled)
    SDL_GetVersion(&linked);

    if (SDL_COMPILEDVERSION !=
        SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
    {
        cerr << "warning: SDL2 compiled and linked version mismatch: "
             << compiled << " " << linked << endl;
    }

    SDL_Window* const window =
        SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 640, 480, ::SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        const char* err_message = SDL_GetError();
        cerr << "error: failed call SDL_CreateWindow: " << err_message << endl;
        SDL_Quit();
        return EXIT_FAILURE;

        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    bool loop = true;
    while (loop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            switch (event.type)
            {
                case SDL_KEYUP:
                    check_input(event);
                    break;
                case SDL_KEYDOWN:
                    check_input(event);
                    break;
                case SDL_QUIT:
                    loop = false;
                    break;
                default:
                    break;
            }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
