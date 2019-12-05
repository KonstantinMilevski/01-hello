#pragma once

#include "engine.hxx"

#include <cassert>
#include <tuple>
#include <vector>

#pragma pack(push, 1)
class texture_gl_es20 final : public texture
{
public:
    /// read png and create texture
    explicit texture_gl_es20(std::string_view path);
    ~texture_gl_es20() override;

    void bind() const
    {
        glBindTexture(GL_TEXTURE_2D, tex_handl);
        GL_CHECK();
    }

    std::uint32_t get_width() const final { return width; }
    std::uint32_t get_height() const final { return height; }

private:
    std::string file_path;
    /// индентификатор текстуры
    GLuint        tex_handl = 0;
    std::uint32_t width     = 0;
    std::uint32_t height    = 0;
};
#pragma pack(pop)
