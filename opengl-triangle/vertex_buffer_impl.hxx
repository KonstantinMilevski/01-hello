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
    const vertex*  data() const final;
    virtual size_t size() const final;

    ~vertex_buffer_impl() final {}

private:
    std::vector<tri2> triangles;
};
