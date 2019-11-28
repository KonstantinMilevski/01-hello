#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

#include "engine.hxx"
using namespace std;

int main()
{
    std::unique_ptr<engine, void (*)(engine*)> eng(create_engine(),
                                                   destroy_engine);

    std::string error = eng->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }
    bool continue_loop = true;
    while (continue_loop)
    {
        int key = 0;

        while (eng->read_input(key))
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
        eng->render_triangle(tr);
        eng->swap_buffer();
    }
    eng->uninitialize();
    return 0;
}
