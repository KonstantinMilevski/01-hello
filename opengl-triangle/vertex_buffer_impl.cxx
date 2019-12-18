#include "vertex_buffer_impl.hxx"

vertex_buffer::~vertex_buffer() {}
vertex_buffer_impl::vertex_buffer_impl(const tri2* tri, std::size_t n)
    : count(static_cast<std::uint32_t>(n * 3))
{
    glGenBuffers(1, &gl_handle);
    GL_CHECK()

    bind();

    GLsizeiptr size_in_bytes = static_cast<GLsizeiptr>(n * 3 * sizeof(v2));

    glBufferData(GL_ARRAY_BUFFER, size_in_bytes, &tri->v[0], GL_STATIC_DRAW);
    GL_CHECK()
}
vertex_buffer_impl::vertex_buffer_impl(const v2* vert, std::size_t n)
    : count(static_cast<std::uint32_t>(n))
{
    glGenBuffers(1, &gl_handle);
    GL_CHECK()

    bind();

    GLsizeiptr size_in_bytes = static_cast<GLsizeiptr>(n * sizeof(v2));

    glBufferData(GL_ARRAY_BUFFER, size_in_bytes, vert, GL_STATIC_DRAW);
    GL_CHECK()
}
