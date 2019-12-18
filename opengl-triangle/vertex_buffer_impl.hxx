#pragma once

#include "engine.hxx"

#include <algorithm>
#include <cassert>
#include <exception>
#include <iostream>

class vertex_buffer_impl final : public vertex_buffer
{
public:
    vertex_buffer_impl(const tri2* tri, std::size_t n);
    vertex_buffer_impl(const v2* vert, std::size_t n);
    void bind() const override
    {
        glBindBuffer(GL_ARRAY_BUFFER, gl_handle);
        GL_CHECK()
    }
    std::uint32_t size() const override { return count; }

    ~vertex_buffer_impl() final {}

private:
    std::uint32_t gl_handle{ 0 };
    std::uint32_t count{ 0 };
};
