#include "04_triangle_render_interpolated.hxx"

#include <SDL.h>

#include <cstdlib>
#include <iostream>

int main(int, char**)
{

    using namespace std;

    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "runtime soft render", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL);
    if (window == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    const color black  = { 0, 0, 0 };
    const color whight = { 255, 255, 255 };
    const color green  = { 0, 255, 0 };

    canvas image;

    triangle_interpolated interpolated_render(image, width, height);
    //////////////////
    struct program : gfx_program
    {
        std::array<color, buffer_size> texture;
        double                         mouse_x{};
        double                         mouse_y{};
        double                         radius{};

        void set_uniforms(const uniforms& a_uniforms) override
        {
            mouse_x = a_uniforms.f0;
            mouse_y = a_uniforms.f1;
            radius  = a_uniforms.f2;
        }
        vertex vertex_shader(const vertex& v_in) override
        {
            ///

            vertex out = v_in;

            double x  = v_in.f0;
            double y  = v_in.f1;
            double dx = (x - mouse_x);
            double dy = (y - mouse_y);
            if (dx * dx + dy * dy < radius * radius * 2)
            {
                out.f0 = x + 1 / (dx + 1);
                out.f1 = y + 1 / (dy + 1);
                out.f2 = 180;
                out.f3 = 0;
                out.f4 = 0;
            }

            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;
            out.r = static_cast<uint8_t>(v_in.f2 * 255);
            out.g = static_cast<uint8_t>(v_in.f3 * 255);
            out.b = static_cast<uint8_t>(v_in.f4 * 255);

            double x  = v_in.f0;
            double y  = v_in.f1;
            double dx = mouse_x - x;
            double dy = mouse_y - y;
            if (dx * dx + dy * dy < radius * radius)
            {
                // make pixel gray if mouse cursor around current pixel with
                // radius
                // gray scale with formula: 0.21 R + 0.72 G + 0.07 B.
                //                double gray = 0.21 * out.r + 0.72 * out.g +
                //                0.07 * out.b; out.r       = gray; out.g =
                //                gray; out.b       = gray;
                color from_texture = sample2d(v_in.f5, v_in.f6);
                out.r += from_texture.r;
                out.g = from_texture.g;
                out.b += from_texture.b;
            }

            return out;
        }
        void set_texture(const std::array<color, buffer_size>& tex)
        {
            texture = tex;
        }

        color sample2d(double u_, double v_)
        {
            uint32_t u = static_cast<uint32_t>(std::round(u_));
            uint32_t v = static_cast<uint32_t>(std::round(v_));

            color c = texture.at(v * width + u);
            return c;
        }
    } program01;
    ///////////////////

    // clang-format off


     //                                 x    y   r  g  b   u    v   &
//                                     f0   f1  f2 f3 f4  f5   f6
//    std::vector<vertex> triangle_v{ {   50,   50,  1, 0, 0,  0,   0,  0 },
//                                    {   50, 189,  0, 1,0,  0,  239, 0 },
//                                    { 279, 189,  0,0, 1, 319, 239, 0 } };
//    // clang-format on
//    std::vector<uint8_t> indexes_v{ 0, 1, 2 };


    void*     pixels = image.data();
    const int depth  = sizeof(color) * 8;
    const int pitch  = width * sizeof(color);
    const int rmask  = 0x000000ff;
    const int gmask  = 0x0000ff00;
    const int bmask  = 0x00ff0000;
    const int amask  = 0;

    interpolated_render.set_gfx_program(program01);

    double mouse_x{};
    double mouse_y{};
    double radius{ 20.0 }; // 20 pixels radius

    bool continue_loop = true;

    while (continue_loop)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                continue_loop = false;
                break;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
            }
            else if (e.type == SDL_MOUSEWHEEL)
            {
                radius *= e.wheel.y;
            }
        }

        interpolated_render.clear(black);
        program01.set_uniforms(uniforms{ mouse_x, mouse_y, radius });

        //interpolated_render.draw_filled_triangle(triangle_v, indexes_v);

    size_t max_x = 20;
    size_t max_y = 20;

    int32_t step_x = (width -40) / max_x;
    int32_t step_y = (height -20) / max_y;

    int32_t last_x=step_x*max_x;
    int32_t last_y=step_y*max_y;

for (size_t y = 0; y <= last_y; y = y + step_y)
{
    for (size_t x = 0; x <= last_x; x = x + step_x)
    {
        vertex v0{ static_cast<double>(x), static_cast<double>(y), 0, 1, 0, 319, 239, 0 };
        vertex v1{static_cast<double>(x), static_cast<double>(y+step_y), 0, 1, 0, 319, 239, 0 };
        interpolated_render.build_line(v0, v1);

    }
}
//size_t x = 0;
for (size_t x = 0; x <= last_x; x = x + step_x)
{

for (size_t y = 0; y <= last_y; y = y + step_y)

    {
        vertex v0{ static_cast<double>(x), static_cast<double>(y), 0, 1, 0, 319, 239, 0 };
        vertex v1{ static_cast<double>(step_x+x), static_cast<double>(y), 0, 1, 0, 319, 239, 0 };
         interpolated_render.build_line(v0, v1);
    }
}

        SDL_Surface* bitmapSurface = SDL_CreateRGBSurfaceFrom(
            pixels, width, height, depth, pitch, rmask, gmask, bmask, amask);
        if (bitmapSurface == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_Texture* bitmapTex =
            SDL_CreateTextureFromSurface(renderer, bitmapSurface);
        if (bitmapTex == nullptr)
        {
            cerr << SDL_GetError() << endl;
            return EXIT_FAILURE;
        }
        SDL_FreeSurface(bitmapSurface);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bitmapTex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(bitmapTex);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return EXIT_SUCCESS;
}
