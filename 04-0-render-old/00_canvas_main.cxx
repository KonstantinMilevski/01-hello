#include "00_canvas.hxx"
#include <iostream>

int main()
{

    const color green = { 0, 255, 0 };

    canvas image;
    std::fill(begin(image), end(image), green);
    std::string file_name = "00-canvas";
    image.save_image(file_name);

    canvas image_loaded;
    image_loaded.load_image(file_name);

    if (image != image_loaded)
    {
        std::cerr << "image != image_loaded\n";
        return 1;
    }
    else
    {
        std::cout << "image == image_loaded\n";
    }

    return 0;
}
