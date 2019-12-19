#include "engine.hxx"
#include "shader_gl_es20.hxx"
#include "texture_gl_es20.hxx"
#include "vertex_buffer_impl.hxx"

#include <algorithm>
#include <cassert>
#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

//#define U(x) (x)
//#define V(x) (1.0f - (x))

// static const GLfloat globBoxVertexData[] = {
//    //   X     Y     Z       U        V
//    // front
//    1.0f,    1.0f,    1.0f, U(1.0f), V(1.0f), -1.0f,   1.0f,    1.0f,
//    U(0.0f), V(1.0f), 1.0f, -1.0f,   1.0f,    U(1.0f), V(0.0f),

//    // ....

//};

/// check opengl function

unsigned long text_width  = 0;
unsigned long text_height = 0;

/// kod for SDL_events
// static std::array<std::string_view, 17> event_names = {
//    /// input events
//    { "left_pressed", "left_released", "right_pressed", "right_released",
//      "up_pressed", "up_released", "down_pressed", "down_released",
//      /// virtual console events
//      "turn_off" }
//};
// std::ostream& operator<<(std::ostream& stream, const event e)
//{
//    std::uint32_t value   = static_cast<std::uint32_t>(e);
//    std::uint32_t minimal = static_cast<std::uint32_t>(event::left_pressed);
//    std::uint32_t maximal = static_cast<std::uint32_t>(event::turn_off);
//    if (value >= minimal && value <= maximal)
//    {
//        stream << event_names[value];
//        return stream;
//    }
//    else
//    {
//        throw std::runtime_error("too big event value");
//    }
//}
struct bind
{
    bind(SDL_Keycode _key, std::string_view _name, keys _k)
        : key(_key)
        , name(_name)
        , select_key(_k)

    {
    }
    SDL_Keycode      key;
    std::string_view name;
    keys             select_key;
};

const std::array<bind, 6> keys_list{
    bind{ SDLK_LEFT, "left", keys::left },
    bind{ SDLK_DOWN, "down", keys::down },
    bind{ SDLK_RIGHT, "right", keys::right },
    bind{ SDLK_UP, "rotate", keys::rotate },
    bind{ SDLK_SPACE, "pause", keys::pause },
    bind{ SDLK_ESCAPE, "exit", keys::exit },
}

;
static bool check_input(const SDL_Event& e, const bind*& result)
{
    using namespace std;

    const auto it =
        find_if(begin(keys_list), end(keys_list),
                [&](const ::bind& b) { return b.key == e.key.keysym.sym; });

    if (it != end(keys_list))
    {
        result = &(*it);
        return true;
    }
    return false;
}
///
std::ostream& operator<<(std::ostream& os, const SDL_version& v)
{
    os << v.major << ':';
    os << v.minor << ':';
    os << v.patch;
    return os;
}

std::istream& operator>>(std::istream& is, vertex& v)
{
    is >> v.x;
    is >> v.y;
    is >> v.tx;
    is >> v.ty;
    return is;
}
std::istream& operator>>(std::istream& is, triangle& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

tri2::tri2()
    : v{ v2(), v2(), v2() }
{
}
tri2::tri2(v2 v01, v2 v02, v2 v03)
    : v{ v2(v01), v2(v02), v2(v03) }
{
}
std::istream& operator>>(std::istream& is, v2& v)
{
    is >> v.pos.x;
    is >> v.pos.y;
    is >> v.uv.x;
    is >> v.uv.y;
    return is;
}
std::istream& operator>>(std::istream& is, tri2& t)
{
    is >> t.v[0];
    is >> t.v[1];
    is >> t.v[2];
    return is;
}

///
vertex_buffer::~vertex_buffer() {}
texture::~texture() {}
engine::~engine() {}
/// engine

/// engine_impl
class engine_impl final : public engine
{
public:
    std::string initialize(std::string_view str) override final
    {

        using namespace std;

        stringstream serr;

        SDL_version compiled = { 0, 0, 0 };
        SDL_version linked   = { 0, 0, 0 };
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        if (SDL_COMPILEDVERSION !=
            SDL_VERSIONNUM(linked.major, linked.minor, linked.patch))
        {
            serr << "warning: SDL2 compiled and linked version mismatch: "
                 << compiled << " " << linked << endl;
        }

        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            serr << "error: failed call SDL_Init: " << SDL_GetError() << endl;
            return serr.str();
        }

        window = SDL_CreateWindow("open-GL test", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, width, heigh,
                                  ::SDL_WINDOW_OPENGL);
        if (window == nullptr)
        {
            serr << "error: failed call SDL_CreateWindow: " << SDL_GetError()
                 << endl;
            return serr.str();
        }

        gl_context = SDL_GL_CreateContext(window);
        if (gl_context == nullptr)
        {
            std::string msg("can't create opengl context: ");
            msg += SDL_GetError();
            serr << msg << endl;
            return serr.str();
        }

        int gl_major_ver = 2;

        int result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_ver);
        SDL_assert(result == 0);
        int gl_minor_ver = 0;
        result =
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_ver);
        SDL_assert(result == 0);

        if (gl_major_ver <= 2 && gl_minor_ver < 1)
        {
            serr << "current context opengl version: " << gl_major_ver << '.'
                 << gl_minor_ver << '\n'
                 << "need opengl version at least: 2.1\n"
                 << std::flush;
            return serr.str();
        }

        if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0)
        {
            std::cerr << "error: failed to initialize glad" << std::endl;
        }
        /// glGenBuffers
        glGenBuffers(1, &gl_default_vbo);
        GL_CHECK()
        glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
        GL_CHECK()
        uint32_t data_size_in_bytes = 0;
        glBufferData(GL_ARRAY_BUFFER, data_size_in_bytes, nullptr,
                     GL_STATIC_DRAW);
        GL_CHECK()
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, nullptr);
        GL_CHECK()

        shader00 = new shader_gl_es20(
            R"(
                attribute vec2 a_position;
                attribute vec2 a_tex_coord;
                //attribute vec4 a_color;
                //varying vec4 v_color;
                varying vec2 v_tex_coord;
                void main()
                {
                v_tex_coord = a_tex_coord;
                //v_color = a_color;
                gl_Position = vec4(a_position, 0.0, 1.0);
                }
                )",
            R"(
                #ifdef GL_ES
                precision mediump float;
                #endif
                varying vec2 v_tex_coord;
                //varying vec4 v_color;
                uniform sampler2D s_texture;
                void main()
                {
                gl_FragColor = texture2D(s_texture, v_tex_coord); //* v_color;
                }
                )",
            { { 0, "a_position" },
              //{ 1, "a_color" },
              { 2, "a_tex_coord" } });

        // turn on rendering with just created shader program
        shader00->use();
        /// end glGenBuffers

        // create vertex shader
        //        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        //        GL_CHECK();
        //        string_view vertex_source = R"(
        //                                        attribute vec2 a_position;
        //                                        attribute vec2 a_tex_coord;
        //                                        varying vec2 v_tex_coord;
        //                                        void main()
        //                                        {
        //                                            v_tex_coord =
        //                                            vec2(a_tex_coord.x,
        //                                            a_tex_coord.y/7.0f);
        //                                            gl_Position =
        //                                            vec4(a_position,
        //                                            0.0, 1.0);
        //                                        }
        //                                        )";
        //        const char* source        = vertex_source.data();
        //        glShaderSource(vertex_shader, 1, &source, nullptr);
        //        GL_CHECK();

        //        glCompileShader(vertex_shader);
        //        GL_CHECK();

        //        GLint compiled_status = 0;
        //        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS,
        //        &compiled_status); GL_CHECK() if (compiled_status == 0)
        //        {
        //            GLint info_len = 0;
        //            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH,
        //            &info_len); GL_CHECK() std::vector<char>
        //            info_chars(static_cast<size_t>(info_len));
        //            glGetShaderInfoLog(vertex_shader, info_len, nullptr,
        //                               info_chars.data());
        //            GL_CHECK()
        //            glDeleteShader(vertex_shader);
        //            GL_CHECK()

        //            serr << "Error compiling shader(vertex)\n"
        //                 << vertex_source << "\n"
        //                 << info_chars.data();
        //            return serr.str();
        //        }

        //        // create fragment shader

        //        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        //        GL_CHECK();
        //        string_view fragment_source = R"(
        //                                        varying vec2 v_tex_coord;
        //                                        uniform sampler2D s_texture;
        //                                        void main()
        //                                        {
        //                                            gl_FragColor =
        //                                            texture2D(s_texture,
        //                                            v_tex_coord);
        //                                        }
        //                                        )";
        //        source                      = fragment_source.data();
        //        glShaderSource(fragment_shader, 1, &source, nullptr);
        //        GL_CHECK();

        //        glCompileShader(fragment_shader);
        //        GL_CHECK();

        //        compiled_status = 0;
        //        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS,
        //        &compiled_status); GL_CHECK() if (compiled_status == 0)
        //        {
        //            GLint info_len = 0;
        //            glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH,
        //            &info_len); GL_CHECK() std::vector<char>
        //            info_chars(static_cast<size_t>(info_len));
        //            glGetShaderInfoLog(fragment_shader, info_len, nullptr,
        //                               info_chars.data());
        //            GL_CHECK()
        //            glDeleteShader(fragment_shader);
        //            GL_CHECK()

        //            serr << "Error compiling shader(fragment)\n"
        //                 << fragment_source << "\n"
        //                 << info_chars.data();
        //            return serr.str();
        //        }

        //        // create program and attach vertex and fragment shaders

        //        program_id_ = glCreateProgram();
        //        GL_CHECK()
        //        if (0 == program_id_)
        //        {
        //            cerr << "error: can't create GL_program";
        //            return serr.str();
        //        }

        //        glAttachShader(program_id_, vertex_shader);
        //        GL_CHECK()

        //        glAttachShader(program_id_, fragment_shader);
        //        GL_CHECK()

        //        glBindAttribLocation(program_id_, 0, "a_position");
        //        GL_CHECK()

        //        glLinkProgram(program_id_);
        //        GL_CHECK()

        //        GLint linked_status = 0;
        //        glGetProgramiv(program_id_, GL_LINK_STATUS, &linked_status);
        //        GL_CHECK()
        //        if (linked_status == 0)
        //        {
        //            GLint infoLen = 0;
        //            glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        //            GL_CHECK()
        //            std::vector<char> infoLog(static_cast<size_t>(infoLen));
        //            glGetProgramInfoLog(program_id_, infoLen, nullptr,
        //            infoLog.data()); GL_CHECK() serr << "Error linking
        //            program:\n" << infoLog.data();
        //            glDeleteProgram(program_id_);
        //            GL_CHECK()
        //            return serr.str();
        //        }
        //        glDeleteShader(vertex_shader);
        //        GL_CHECK()
        //        glDeleteShader(fragment_shader);
        //        GL_CHECK()
        //        glUseProgram(program_id_);
        //        GL_CHECK()

        //        ///
        //        int location = glGetUniformLocation(program_id_, "s_texture");
        //        GL_CHECK();
        //        assert(-1 != location);
        //        int texture_unit = 0;
        //        glActiveTexture(GL_TEXTURE0 + texture_unit);
        //        GL_CHECK();
        //        unsigned long w{ 0 };
        //        unsigned long h{ 0 };
        //        if (!load_texture("blocks1.png", w, h))
        //        {
        //            return "failed load texture\n";
        //        }

        //        //
        //        http://www.khronos.org/opengles/sdk/docs/man/xhtml/glUniform.xml
        //        glUniform1i(location, 0 + texture_unit);
        //        GL_CHECK();

        //        // glEnable(SDL_GL_DOUBLEBUFFER);
        //        glEnable(GL_DEPTH_TEST);
        //        // glDepthFunc(GL_LESS);

        //        //        glEnable(GL_BLEND);
        //        //        GL_CHECK();
        //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //        GL_CHECK();
        //        ///
        //        glEnable(GL_DEPTH_TEST);
        //        GL_CHECK()

        return "";
    }
    void uninitialize() override final
    {
        // glDeleteProgram(program_id_);
        delete shader00;
        SDL_GL_DeleteContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    bool load_texture(std::string_view path, unsigned long w,
                      unsigned long h) final
    {
        //        // size_t                 i =
        //        std::filesystem::file_size(path); std::vector<std::byte>
        //        png_file_in_memory; std::ifstream          ifs(path.data(),
        //        std::ios::binary); if (!ifs)
        //        {
        //            return false;
        //        }
        //        ifs.seekg(0, std::ios_base::end);
        //        size_t pos_end_file = static_cast<size_t>(ifs.tellg());
        //        png_file_in_memory.resize(pos_end_file);
        //        ifs.seekg(0, std::ios_base::beg);
        //        ifs.read(reinterpret_cast<char*>(png_file_in_memory.data()),
        //                 static_cast<std::streamsize>(png_file_in_memory.size()));
        //        if (!ifs.good())
        //        {
        //            return false;
        //        }
        //        // std::reverse(png_file_in_memory.begin(),
        //        // png_file_in_memory.end());

        //        //        unsigned long          w{ 0 };
        //        //        unsigned long          h{ 0 };
        //        std::vector<std::byte> image;
        //        int error = decodePNG(image, w, h, &png_file_in_memory[0],
        //                              png_file_in_memory.size(), false);
        //        if (0 != error)
        //        {
        //            std::cerr << "error: " << error << std::endl;
        //            return false;
        //        }
        //        text_width       = w;
        //        text_height      = h;
        //        GLuint tex_handl = 0;
        //        /// gen name for text
        //        glGenTextures(1, &tex_handl);
        //        GL_CHECK()
        //        /// set current text
        //        glBindTexture(GL_TEXTURE_2D, tex_handl);
        //        GL_CHECK()

        //        GLint mipmap_level = 0;
        //        GLint border       = 0;
        //        //происходит выделение памяти и загрузка в нее наших данных.
        //        // clang-format off
        //        glTexImage2D(GL_TEXTURE_2D, // Specifies the target texture of
        //        the active texture unit
        //                     mipmap_level,  // Specifies the level-of-detail
        //                     number. Level 0 is the base image level GL_RGBA,
        //                     // Specifies the internal format of the texture
        //                     static_cast<GLsizei>(w),
        //                     static_cast<GLsizei>(h),
        //                     border,        // Specifies the width of the
        //                     border. Must be 0. For GLES 2.0 GL_RGBA,       //
        //                     Specifies the format of the texel data. Must
        //                     match internalformat GL_UNSIGNED_BYTE, //
        //                     Specifies the data type of the texel data
        //                     &image[0]);    // Specifies a pointer to the
        //                     image data in memory
        //        // clang-format on
        //        GL_CHECK()
        //        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        //        //        GL_CLAMP_TO_EDGE); GL_CHECK()
        //        glTexParameteri(GL_TEXTURE_2D,
        //        //        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); GL_CHECK()

        //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        //        GL_NEAREST); GL_CHECK() glTexParameteri(GL_TEXTURE_2D,
        //        GL_TEXTURE_MAG_FILTER, GL_NEAREST); GL_CHECK()

        return true;
    }
    void render_triangle(triangle& t) override
    {

        //        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        //        sizeof(vertex),
        //                              &t.v[0]);
        //        GL_CHECK()
        //        // first attribute - vertex
        //        glEnableVertexAttribArray(0);
        //        GL_CHECK()
        //        glValidateProgram(program_id_);
        //        GL_CHECK()
        //        // Check the validate status
        //        GLint validate_status = 0;
        //        glGetProgramiv(program_id_, GL_VALIDATE_STATUS,
        //        &validate_status); GL_CHECK() if (validate_status == GL_FALSE)
        //        {
        //            GLint infoLen = 0;
        //            glGetProgramiv(program_id_, GL_INFO_LOG_LENGTH, &infoLen);
        //            GL_CHECK()
        //            std::vector<char> infoLog(static_cast<size_t>(infoLen));
        //            glGetProgramInfoLog(program_id_, infoLen, nullptr,
        //            infoLog.data()); GL_CHECK() std::cerr << "Error linking
        //            program:\n" << infoLog.data(); throw
        //            std::runtime_error("error");
        //        }
        //        glDrawArrays(GL_TRIANGLES, 0, 3);
        //        GL_CHECK()
    }
    void render_text_triangle(triangle& t) override
    {
        //        triangle t = transform_coord_to_GL(text_width, text_height,
        //        t_in);
        //        // vertex coordinates
        //        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        //        sizeof(vertex),
        //                              &t.v[0].x);
        //        GL_CHECK();
        //        glEnableVertexAttribArray(0);
        //        GL_CHECK();

        //        // texture coordinates
        //        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        //        sizeof(vertex),
        //                              &t.v[0].tx);
        //        GL_CHECK();
        //        glEnableVertexAttribArray(1);
        //        GL_CHECK();

        //        glDrawArrays(GL_TRIANGLES, 0, 3);
        //        GL_CHECK();
    }
    void render_two_triangles(const std::vector<triangle>& v_in) override final
    {

        //        std::vector<triangle> t;
        //        for (auto var : v_in)
        //        {

        //            triangle t1 = transform_coord_to_GL(text_width,
        //            text_height, var); t.push_back(t1);
        //        }

        //        // vertex coordinates
        //        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        //        sizeof(vertex),
        //                              &t[0].v[0].x);
        //        GL_CHECK();
        //        glEnableVertexAttribArray(0);
        //        GL_CHECK();

        //        // texture coordinates
        //        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        //        sizeof(vertex),
        //                              &t[0].v[0].tx);
        //        GL_CHECK();
        //        glEnableVertexAttribArray(1);
        //        GL_CHECK();
        //        glDrawArrays(GL_TRIANGLES, 0, v_in.size() * 3);
        //        GL_CHECK()
    }

    void render_tetris(const vertex_buffer& buff, texture* tex) final
    {

        shader00->use();
        texture_gl_es20* texture = static_cast<texture_gl_es20*>(tex);
        texture->bind();
        shader00->set_uniform("s_texture", texture);

        /// generate a new VBO and get the associated ID
        glGenBuffers(1, &gl_default_vbo);
        GL_CHECK()

        assert(gl_default_vbo != 0);
        glBindBuffer(GL_ARRAY_BUFFER, gl_default_vbo);
        GL_CHECK()

        const v2* t = buff.data();
        uint32_t  data_size_in_bytes =
            static_cast<uint32_t>(buff.size() * sizeof(v2));
        /// copy the data into the buffer object, when the buffer has been
        /// initialized
        glBufferData(
            GL_ARRAY_BUFFER, data_size_in_bytes,
            t, /// Specifies a pointer to data that will be copied into the data
            /// store for initialization, or NULL if no data is to be copied.
            GL_DYNAMIC_DRAW /// Specifies the expected usage pattern of the data
            /// store
        );
        GL_CHECK()
        /// update a subset of a buffer object's data store
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size_in_bytes, t);
        GL_CHECK()

        // positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(v2), nullptr);

        GL_CHECK()
        glEnableVertexAttribArray(0);
        GL_CHECK()
        // colors
        //        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        //        sizeof(t.v[0]),
        //                              &t.v[0].c);
        //        GL_CHECK()
        //        glEnableVertexAttribArray(1);
        //        GL_CHECK()

        // texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(v2),
                              reinterpret_cast<void*>(sizeof(v2::pos)));
        //        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        //        sizeof(t.v[0]),
        //                              &t.v[0].uv);
        GL_CHECK()
        glEnableVertexAttribArray(2);
        GL_CHECK()

        glDrawArrays(GL_TRIANGLES, 0, buff.size());
        GL_CHECK()

        //        glDisableVertexAttribArray(1);
        //        GL_CHECK()
        glDisableVertexAttribArray(2);
        GL_CHECK()
    }

    void swap_buffer() override final
    {
        SDL_GL_SwapWindow(window);

        glClearColor(0.3f, 0.3f, 1.0f, 0.0f);
        GL_CHECK()
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        GL_CHECK()
    }

    bool read_event(keys& key) override final
    {
        using namespace std;
        SDL_Event sdl_event;
        if (SDL_PollEvent(&sdl_event))
        {
            const ::bind* binding = nullptr;

            if (sdl_event.type == SDL_QUIT)
            {
                key = keys::exit;
                return true;
            }
            else if (sdl_event.type == SDL_KEYDOWN)
            {
                if (check_input(sdl_event, binding))
                {
                    key = binding->select_key;
                    return true;
                }
            }
            else if (sdl_event.type == SDL_KEYUP)
            {
                if (check_input(sdl_event, binding))
                {
                    key = binding->select_key;
                    return true;
                }
            }

            return false;
        }
        return true;
    }

    texture* create_texture(std::string_view path) final
    {
        return new texture_gl_es20(path);
    }
    texture* create_texture_part(std::string_view path, const size_t width,
                                 const size_t height)
    {
        return new texture_gl_es20(path, width, height);
    }
    void destroy_texture(texture* t) final { delete t; }

    /// create_vertex_buffer
    vertex_buffer* create_vertex_buffer(const tri2* triangles, std::size_t n)
    {
        assert(triangles != nullptr);
        return new vertex_buffer_impl(triangles, n);
    }
    //    vertex_buffer* create_vertex_buffer(const v2* vert, std::size_t count)
    //    {
    //        assert(vert != nullptr);
    //        return new vertex_buffer_impl(vert, count);
    //    }
    void destroy_vertex_buffer(vertex_buffer* buffer) { delete buffer; }

private:
    SDL_Window*     window         = nullptr;
    SDL_GLContext   gl_context     = nullptr;
    GLuint          program_id_    = 0;
    shader_gl_es20* shader00       = nullptr;
    uint32_t        gl_default_vbo = 0;
};

static bool already_exist = false;
engine*     create_engine()
{
    if (true == already_exist)
    {
        throw std::runtime_error("engine allredy exist");
    }
    engine* eng   = new engine_impl();
    already_exist = true;
    return eng;
}
void destroy_engine(engine* eng)
{
    if (false == already_exist)
    {
        throw std::runtime_error("engine no exist");
    }
    if (nullptr == eng)
    {
        throw std::runtime_error("engine is null");
    }
    delete eng;
}
