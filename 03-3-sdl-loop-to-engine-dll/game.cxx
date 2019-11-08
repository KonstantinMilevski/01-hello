#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "engine.hxx"

int main()
{
    std::unique_ptr<gm::engine, void (*)(gm::engine*)> engine(gm::buildEngine(),
                                                              gm::delEngine);
    std::string err = engine->initialize("");
    if (!err.empty())
    {
        std::cerr << err;
        return EXIT_FAILURE;
    }
    bool loop = true;
    while (loop)
    {
        gm::event e;
        if (engine->readInput(e))
        {
            std::cout << e << std::endl;
            if (e == gm::event::turn_off)
            {
                loop == false;
                break;
            }
        }
    }
    engine->uninitialize();
    return EXIT_SUCCESS;
}
