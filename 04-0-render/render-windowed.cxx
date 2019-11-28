#include "interpolateded_triangle_render.hxx"

#include <SDL.h>

#include <cstdlib>
#include <iostream>

int main(int, char**)
{
    using namespace std;
    const size_t  max_x  = 12;
    const size_t  max_y  = 16;
    const int32_t step_x = (width - 1) / max_x;
    const int32_t step_y = (height - 1) / max_y;

    if (0 != SDL_Init(SDL_INIT_EVERYTHING))
    {
        cerr << SDL_GetError() << endl;
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow(
        "runtime soft render_mk", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
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

    const color black = { 0, 0, 0 };

    canvas image;

    interpolateded_triangle_render interpolated_render(image, width, height);

    struct program : gfx_program
    {
        double mouse_x{};
        double mouse_y{};
        double radius{};

        void set_uniforms(const uniforms& a_uniforms) override
        {
            mouse_x = a_uniforms.f0;
            mouse_y = a_uniforms.f1;
            radius  = a_uniforms.f2;
        }
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            //            double x = out.f0;
            //            double y = out.f1;

            //            out.f0 = x;
            //            out.f1 = y;

            ////

            double x    = v_in.f0;
            double y    = v_in.f1;
            double dx   = (x - mouse_x);
            double dy   = (y - mouse_y);
            double diag = std::sqrt(step_x * step_x + step_y * step_y);
            if (dx * dx + dy * dy < radius * radius)
            {

                double length_v_m = std::sqrt(dx * dx + dy * dy);
                if (radius > 0)
                {
                    double radius_x  = mouse_x + radius * dx / length_v_m;
                    double radius_y  = mouse_y + radius * dy / length_v_m;
                    double radius_2x = mouse_x + radius * dx / (length_v_m * 2);
                    double radius_2y = mouse_y + radius * dy / (length_v_m * 2);
                    if (length_v_m != 0 && length_v_m < radius)
                    {
                        // find position on circle = radius

                        out.f0 = (x + radius_x) / 2;
                        out.f1 = (y + radius_y) / 2;
                    }
                    if (length_v_m != 0 && length_v_m < radius && radius < diag)

                    {
                        out.f0 = radius_2x;
                        out.f1 = radius_2y;
                    }
                }
                if (radius <= 0)
                {
                    double radius_x = mouse_x - radius * dx / length_v_m;
                    double radius_y = mouse_y - radius * dy / length_v_m;
                    double radius_2x =
                        mouse_x - 1 / radius * dx / (length_v_m * 2);
                    double radius_2y =
                        mouse_y - 1 / radius * dy / (length_v_m * 2);
                    out.f0 = mouse_x + (-x + radius_x) / 10;
                    out.f1 = mouse_y + (-y + radius_y) / 10;
                }

                out.f2 = 180;
                out.f3 = 0;
                out.f4 = 0;
            }
            // check out of buffer_size
            if (out.f1 >= max_y * step_y - 1)
                out.f1 = max_y * step_y;
            if (out.f0 >= max_x * step_x)
                out.f0 = max_x * step_x - 1;
            if (out.f1 < 0)
                out.f1 = 0;
            if (out.f0 < 0)
                out.f0 = 0;

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
            if (dx * dx + dy * dy == radius * radius)
            {
                if (radius > 0)
                {
                    // make pixel gray if mouse cursor around current pixel with
                    // radius
                    // gray scale with formula: 0.21 R + 0.72 G + 0.07 B.
                    double gray = 0.21 * out.r + 0.72 * out.g + 0.07 * out.b;
                    //                out.r       = gray;
                    //                out.g       = gray;
                    //                out.b       = gray;
                }
                if (radius < 0)
                    out = { 0, 0, 255 };
            }

            return out;
        }
    } program01;

    //    std::vector<vertex>  triangle_v{ { 0, 0, 1, 0, 0, 0, 0, 0 },
    //                                    { 0, 239, 0, 1, 0, 0, 239, 0 },
    //                                    { 319, 239, 0, 0, 1, 319, 239, 0 }
    //                                    };
    //    std::vector<uint8_t> indexes_v{ 0, 1, 2 };

    std::vector<vertex>  triangle_v;
    std::vector<uint8_t> indexes_v;

    draw_cells(max_x, max_y, triangle_v, indexes_v);

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
    double radius = { 30 }; // 20 pixels radius

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
                if (e.wheel.y > 0) // scroll up
                {
                    radius += 1;
                }
                else if (e.wheel.y < 0) // scroll down
                {
                    radius -= 1;
                }
        }

        interpolated_render.clear(black);
        program01.set_uniforms(uniforms{ mouse_x, mouse_y, radius });

        interpolated_render.draw_interpolated_triangle(triangle_v, indexes_v);

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
