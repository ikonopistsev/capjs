#pragma once

#include "btdef/string.hpp"

namespace captor {

typedef btdef::string string_type;

void write_value(string_type& result, char t, const char* val, std::size_t vs);

void write_value(string_type& result, char d, const double* val);

void write_value(string_type& result, char t, const long long* val);

void write_value(string_type& result, const char* val, std::size_t vs,
                 const char* key, std::size_t ks);

void write_value(string_type& result, const double* val,
                 const char* key, std::size_t ks);

void write_value(string_type& result, const long long* val,
                 const char* key, std::size_t ks);

void write_key_value(string_type& result, const char* val, std::size_t vs,
    const char* key, std::size_t ks);

void write_key_value(string_type& result, const double* val,
    const char* key, std::size_t ks);

void write_key_value(string_type& result, const long long* val,
    const char* key, std::size_t ks);

} // namespace captor
