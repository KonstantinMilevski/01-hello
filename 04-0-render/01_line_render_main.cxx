#include "01_line_render.hxx"

int main()
{
    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };

    canvas image;

    line_render render(image, width, height);

    render.clear(black);

    render.draw_line(position{ 0, 0 }, position{ width - 1, height - 1 },
                     white);

    image.save_image("01-line.ppm");

    return 0;
}
