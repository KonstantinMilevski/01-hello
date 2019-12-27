#include "vertex_buffer_impl.hxx"

vertex_buffer_impl::vertex_buffer_impl(const tri2* tri, std::size_t n)
    : vertexes(n)
{
    assert(tri != nullptr);
    std::copy_n(tri, n, begin(vertexes));
}

const vertex* vertex_buffer_impl::data() const
{
    return vertexes.data();
    ;
}
size_t vertex_buffer_impl::size() const
{
    return vertexes.size();
}
