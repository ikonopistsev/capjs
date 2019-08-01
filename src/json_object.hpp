#pragma once

#include "json_write.hpp"

namespace captor {

class json_object
{
    string_type text_{};

public:
    json_object() = default;

    void start();

    void end();

    void add(const char* key, std::size_t ks,
             const char* val, std::size_t vs);

    void add(const char* key, std::size_t ks, const double* val);

    void add(const char* key, std::size_t ks, const long long* val);

    const char* data() const noexcept
    {
        return text_.data();
    }

    std::size_t size() const noexcept
    {
        return text_.size();
    }
};

} // namespace captor
