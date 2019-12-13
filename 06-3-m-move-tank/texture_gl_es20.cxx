#include "texture_gl_es20.hxx"
#include "picopng.hxx"

texture_gl_es20::texture_gl_es20(std::string_view path)
    : file_path(path)
{
    std::vector<unsigned char> png_file_in_memory;
    std::ifstream              ifs(path.data(), std::ios_base::binary);
    if (!ifs)
    {
        throw std::runtime_error("can't load texture");
    }
    ifs.seekg(0, std::ios_base::end);
    std::streamoff pos_in_file = ifs.tellg();
    png_file_in_memory.resize(static_cast<size_t>(pos_in_file));
    ifs.seekg(0, std::ios_base::beg);
    if (!ifs)
    {
        throw std::runtime_error("can't load texture");
    }

    ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()), pos_in_file);
    if (!ifs.good())
    {
        throw std::runtime_error("can't load texture");
    }

    std::vector<unsigned char> image;
    unsigned long              w = 0;
    unsigned long              h = 0;
    int error = decodePNG(image, w, h, &png_file_in_memory[0],
                          png_file_in_memory.size(), false);

    // if there's an error, display it
    if (error != 0)
    {
        std::cerr << "error: " << error << std::endl;
        throw std::runtime_error("can't load texture");
    }

    ///Первый указывает количество имен-идентификаторов текстур,
    ///которые нужно создать. Второй параметр - указатель на массив элементов
    ///типа GLuint.

    glGenTextures(1, &tex_handl);
    GL_CHECK();
    //в таргет  GL_TEXTURE_2D помещаем объект текстуры, делаем ее активной.
    glBindTexture(GL_TEXTURE_2D, tex_handl);
    GL_CHECK();

    GLint   mipmap_level = 0;
    GLint   border       = 0;
    GLsizei width_       = static_cast<GLsizei>(w);
    GLsizei height_      = static_cast<GLsizei>(h);
    // send texture OPenGL
    glTexImage2D(GL_TEXTURE_2D, mipmap_level, GL_RGBA, width_, height_, border,
                 GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    GL_CHECK();
    //установить параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK();
}

texture_gl_es20::~texture_gl_es20()
{
    glDeleteTextures(1, &tex_handl);
    GL_CHECK();
}
