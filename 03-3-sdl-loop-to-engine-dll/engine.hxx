#pragma once
#include "SDL2/SDL.h"
#include <iosfwd>
#include <string>
#include <string_view>
namespace gm
{
enum class event
{
    /// input events
    left_pressed,
    left_released,
    right_pressed,
    right_released,
    up_pressed,
    up_released,
    down_pressed,
    down_released,
    select_pressed,
    select_released,
    start_pressed,
    start_released,
    button1_pressed,
    button1_released,
    button2_pressed,
    button2_released,
    /// virtual console events
    turn_off
};
std::ostream& operator<<(std::ostream&, event&);

class engine;

engine* buildEngine();
void    delEngine(engine*);

class engine
{
public:
    //    virtual ~engine();
    //    /// create main window
    //    /// on success return empty string
    //    virtual std::string initialize(std::string_view config) = 0;
    //    /// pool event from input queue
    //    /// return true if event was written
    //    virtual bool read_input(event& e) = 0;
    //    virtual void uninitialize()       = 0;
    /// create main windows
    virtual std::string initialize(std::string_view config) = 0;
    virtual void        uninitialize()                      = 0;
    /// pool event from input queue
    /// return true if event was written
    virtual bool readInput(event&) = 0;
    virtual ~engine();
};

} // namespace gm
