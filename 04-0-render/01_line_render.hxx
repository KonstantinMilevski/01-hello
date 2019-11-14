#pragma once
#include "00_canvas.hxx"

struct line_render : irender
{
    line_render(canvas& buffer, size_t width, size_t height);
    // clang-format off
    void   clear(const color&) override; //fill image
    void   set_pixel(const position&, const color&) override; //set color for point
    pixels pixels_positions(const position&, const position&) override; //create vector of points in line
    void   draw_line(const position&, const position&, const color&);//set color for points in line
    // clang-format on
private:
    canvas&      buffer_;
    const size_t w;
    const size_t h;
};
