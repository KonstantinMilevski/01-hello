#include "canvas.hxx"

int main()
{
    using namespace std;
    canvas      image;
    const color black         = { 0, 0, 0 };
    const color green         = { 0, 255, 0 };
    string      canvas_render = "01.ppm ";
    fill(begin(image), end(image), black);
    image.save_image(canvas_render);

    canvas image_load;
    image_load.load_image(canvas_render);

    if (image_load == image)
    {
        std::cerr << "image == image_loaded\n";
        return EXIT_SUCCESS;
    }
    else
    {
        std::cerr << "image != image_loaded\n";
        return EXIT_FAILURE;
    }
}
