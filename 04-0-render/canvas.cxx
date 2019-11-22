#include "canvas.hxx"

irender::~irender() {}

bool operator==(const position& left, const position& right)
{
    return (left.x == right.x && left.y == right.y);
}

bool operator==(const color& left, const color& right)
{
    return (left.r == right.r && left.g == right.g && left.b == right.b);
}

constexpr size_t color_size = sizeof(color);
static_assert(3 == color_size, "24 bit per pixel(r,g,b)");

position operator-(const position& left, const position& right)
{

    return { right.x - left.x, right.y - left.y };
}
