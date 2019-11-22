#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr size_t width  = 320;
constexpr size_t height = 240;

struct position
{
    int32_t x;
    int32_t y;
    double  length() { return std::sqrt(x * x + y * y); }
};

bool operator==(const position& left, const position& right);

position operator-(const position& left, const position& right);

const size_t buffer_size = width * height;
using pixels             = std::vector<position>;

struct color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
bool operator==(const color&, const color&);

struct canvas : std::array<color, buffer_size>
{
    void save_image(const std::string& file_name)
    {
        std::ofstream out_file;
        out_file.exceptions(std::ios_base::failbit);
        out_file.open(file_name, std::ios_base::binary);
        out_file << "P6\n" << width << ' ' << height << ' ' << 255 << '\n';
        std::streamsize buf_size =
            static_cast<std::streamsize>(sizeof(color) * this->size());
        out_file.write(reinterpret_cast<const char*>(this), buf_size);
        out_file.close();
    }
    void load_image(const std::string& file_name)
    {
        using namespace std;
        ifstream file_in;
        file_in.exceptions(ios_base::failbit);
        file_in.open(file_name, ios_base::binary);
        string type;
        int    w, h;
        string color_size;
        file_in >> type >> w >> h >> color_size >> ws;
        if (size() != w * h)
        {
            throw runtime_error("dif size value");
        }
        file_in.read(reinterpret_cast<char*>(this),
                     static_cast<std::streamsize>(sizeof(color) * buffer_size));
        file_in.close();
    }
};

struct irender
{
    virtual void   clear(color c)                                 = 0;
    virtual pixels pixels_position(const position& start,
                                   const position& end)           = 0;
    virtual void   set_pixel(const position& pos, const color& c) = 0;
    ~irender();
};
