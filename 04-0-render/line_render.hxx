#include "canvas.hxx"

struct line_render : irender
{
    line_render(canvas& buffer, size_t width, size_t height);
    void   clear(color c) override;
    void   set_pixel(const position& pos, const color& c) override;
    pixels pixels_position(const position& start, const position& end) override;
    void draw_line(const position& start, const position& end, const color& c);

private:
    canvas& buffer_;
    size_t  w;
    size_t  h;
};
