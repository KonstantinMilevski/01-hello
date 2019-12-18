#pragma once

#include "engine.hxx"

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>

/// texture
#pragma pack(push, 4)
class texture_gl_es20 final : public texture
{
public:
    explicit texture_gl_es20(std::string_view path);
    texture_gl_es20(std::string_view path, const size_t w_new,
                    const size_t h_new);
    ~texture_gl_es20() override;

    void bind() const override;

    std::uint32_t get_width() const final { return width; }
    std::uint32_t get_height() const final { return height; }

private:
    void gen_texture_from_pixels(const void* pixels, const size_t width,
                                 const size_t height);

    std::string   file_path;
    GLuint        tex_handl = 0;
    std::uint32_t width     = 0;
    std::uint32_t height    = 0;
};
#pragma pack(pop)
