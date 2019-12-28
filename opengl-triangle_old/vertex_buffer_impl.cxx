#include "vertex_buffer_impl.hxx"

vertex_buffer_impl::vertex_buffer_impl(const tri2* tri, std::size_t n)
    : triangles(n)
{
    assert(tri != nullptr);
    for (size_t i = 0; i < n; ++i)
    {
        triangles.push_back(tri[i]);
    }
}

const v2* vertex_buffer_impl::data() const
{
    return &triangles.data()->v[0];
}
size_t vertex_buffer_impl::size() const
{
    return triangles.size() * 3;
}
