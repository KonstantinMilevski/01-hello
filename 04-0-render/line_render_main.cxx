#include "line_render.hxx"

int main()
{
    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };
    canvas      image;
    line_render line(image, width, height);
    line.clear(black);
    line.draw_line(position{ 0, 0 }, position{ width - 1, height - 1 }, green);

    std::string file_name = "02.ppm ";
    image.save_image(file_name);

    return EXIT_SUCCESS;
}
