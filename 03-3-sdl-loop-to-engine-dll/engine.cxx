#include "engine.hxx"

#include <algorithm>
#include <array>
#include <exception>
#include <iosfwd>
#include <string>
#include <string_view>

namespace gm
{
static std::array<std::string_view, 17> event_names = {
    { /// input events
      "left_pressed", "left_released", "right_pressed", "right_released",
      "up_pressed", "up_released", "down_pressed", "down_released",
      "select_pressed", "select_released", "start_pressed", "start_released",
      "button1_pressed", "button1_released", "button2_pressed",
      "button2_released",
      /// virtual console events
      "turn_off" }
};
std::ostream& operator<<(std::ostream& os, event& e)
{
    using namespace std;
    unsigned int value = static_cast<unsigned int>(e);
    unsigned int min   = static_cast<unsigned int>(event::left_pressed);
    unsigned int max   = static_cast<unsigned int>(event::turn_off);
    if (value >= min && value <= max)
    {
        os << event_names[value];
        return os;
    }
    else
    {
        throw out_of_range("e out_of_range");
    }
}

struct bind
{
    bind(SDL_Keycode _key, std::string_view _name, event _key_pressed,
         event _key_released)
        : key(_key)
        , name(_name)
        , key_pressed(_key_pressed)
        , key_released(_key_released)
    {
    }
    SDL_Keycode      key;
    std::string_view name;
    event            key_pressed;
    event            key_released;
};
const std::array<bind, 8> keys{
    { { SDLK_w, "up", event::up_pressed, event::up_released },
      { SDLK_a, "left", event::left_pressed, event::left_released },
      { SDLK_s, "down", event::down_pressed, event::down_released },
      { SDLK_d, "right", event::right_pressed, event::right_released },
      { SDLK_LCTRL, "button1", event::button1_pressed,
        event::button1_released },
      { SDLK_SPACE, "button2", event::button2_pressed,
        event::button2_released },
      { SDLK_ESCAPE, "select", event::select_pressed, event::select_released },
      { SDLK_RETURN, "start", event::start_pressed, event::start_released } }
};

static bool checkInput(const SDL_Event& e, const bind*& result)
{

    const auto it = std::find_if(begin(keys), end(keys), [&](const bind& b) {
        return b.key == e.key.keysym.sym;
    });
    if (it != end(keys))
    {
        result = &(*it);

        return true;
    }
    return false;
}

class engine_run final : public engine
{
    std::string initialize(std::string_view /*config*/) final
    {
        const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
        if (init_result != 0)
        {
            return "can't init window";
        }
        SDL_Window* const window = SDL_CreateWindow(
            "name", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,
            ::SDL_WINDOW_OPENGL);
        if (window == nullptr)
        {
            return "can't create window";
        }

        return "";
    }
    void uninitialize() final { SDL_Quit(); }
    bool readInput(event& ev) final
    {
        SDL_Event   sdl_event;
        const bind* binding = nullptr;
        if (SDL_PollEvent(&sdl_event))
        {
            if (sdl_event.type == SDL_QUIT)
            {
                ev = event::turn_off;
                return true;
            }

            else if (sdl_event.type == SDL_KEYDOWN)
            {
                if (checkInput(sdl_event, binding))
                {
                    ev = binding->key_pressed;
                    return true;
                }
            }

            else if (sdl_event.type == SDL_KEYUP)
            {
                if (checkInput(sdl_event, binding))
                {
                    ev = binding->key_released;
                    return true;
                }
            }
        }

        return false;
    }
};
engine::~engine() {}
static bool eng_exist = false;
engine*     buildEngine()
{
    if (eng_exist == true)
    {
        throw std::runtime_error("Engine allready exist");
    }
    engine* e = new engine_run;
    eng_exist = true;
    return e;
}

void delEngine(engine* e)
{
    if (eng_exist == false)
    {
        throw std::runtime_error("Engine no exist");
    }
    if (e == nullptr)
    {
        throw std::runtime_error("Engine is nullptr");
    }
    delete e;
}

} // namespace gm
