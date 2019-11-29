#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

#include "engine.hxx"

using namespace std;

int main()
{
    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    const std::string error = engine->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }
    bool continue_loop = true;
    while (continue_loop)
    {
        int key = 0;

        while (engine->read_input(key))
        {
            switch (key)
            {
                case 0:
                    continue_loop = false;
                    break;
                default:
                    break;
            }
        }

        std::ifstream file("vertexes.txt");
        assert(!!file);

        triangle tr;
        file >> tr;

        engine->render_triangle(tr);

        file >> tr;
        engine->render_triangle(tr);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}
