#include "json_write.hpp"

namespace captor {

static inline void write_null(string_type& result)
{
    static const ref::string empty(std::cref("null"));
    result += empty;
}

// конвертация строк в вещественное число
static inline void write_as_real(string_type& result, const char* val)
{
    result += utility::to_text(std::atof(val));
}

// конвертация строк в вещественное число через std
static inline void write_as_realstd(string_type& result, const char* val)
{
    result += std::to_string(std::atof(val));
}

// вывод не экранированной строки
static inline void write_raw(string_type& result,
    const char* val, std::size_t vs)
{
    result.append(val, vs);
}

void write_escape(string_type& result, const char* val, std::size_t vs)
{
    auto e = val + vs;
    while (val != e)
    {
        auto c = *val++;
        switch (c)
        {
            case '"':
                result += '\\';
                result += '\"';
            break;
            case '\\':
                result += '\\';
                result += '\\';
            break;
            case '\b':
                result += '\\';
                result += 'b';
            break;
            case '\f':
                result += '\\';
                result += 'f';
            break;
            case '\n':
                result += '\\';
                result += 'n';
            break;
            case '\r':
                result += '\\';
                result += 'r';
            break;
            case '\t':
                result += '\\';
                result += 't';
            break;
            default:
                if (('\x00' <= c) && (c <= '\x1f'))
                {
                    result += '\\';
                    result += 'u';
                    result += '0';
                    result += '0';
                    result += utility::to_hex(c);
                } else {
                    result += c;
                }
        }
    }
}

// вывод не экранированной строки
static inline void write_unescape(string_type& result,
    const char* val, std::size_t vs)
{
    result += '\"';
    result.append(val, vs);
    result += '\"';
}

// стандартный вывод строки
static inline void write_value(string_type& result,
    const char* val, std::size_t vs)
{
    result += '\"';
    write_escape(result, val, vs);
    result += '\"';
}

void write_value(string_type& result, char t, const char* val, std::size_t vs)
{
    if (val)
    {
        switch (t)
        {
        case 'f':
            write_as_real(result, val);
            return;
        case 'r':
            write_unescape(result, val, vs);
            return;
        case 's':
            write_as_realstd(result, val);
            return;
        case 'x':
            write_raw(result, val, vs);
            return;
        default:
            write_value(result, val, vs);
        };
    }
    else
        write_null(result);
}

// стандартный вывод вещественного числа
static inline void write(string_type& result, double val)
{
    result += utility::to_text(val);
}

// стандартный вывод числа с точностью
static inline void write(string_type& result, double val, std::size_t exp)
{
    result += utility::to_text(val, exp);
}

// стандартный вывод числа через std
static inline void write_std(string_type& result, double val)
{
    result += std::to_string(val);
}

void write_value(string_type& result, char d, const double* val)
{
    if (val)
    {
        if (('0' <= d) && (d <= '9'))
            write(result, *val, static_cast<std::size_t>(d - '0'));
        else if (('A' <= d) && (d <= 'F'))
            write(result, *val, static_cast<std::size_t>(d - 'A') + 10);
        else if (('a' <= d) && (d <= 'f'))
            write(result, *val, static_cast<std::size_t>(d - 'a') + 10);
        else if ((d == 'S') || (d == 's'))
            write_std(result, *val);
        else
            write(result, *val);
    }
    else
        write_null(result);
}

// стандартный вывод целого числа
static inline void write(string_type& result, long long val)
{
    result += utility::to_text(static_cast<std::int64_t>(val));
}

static inline void write_time(string_type& result, long long val)
{
    auto t = static_cast<utility::date::value_t>(val);
    result += '\"';
    result += utility::date(t).json_text();
    result += '\"';
}

// вывод времени
static inline void write_unixtime(string_type& result, long long val)
{
    write_time(result, val * 1000);
}

void write_value(string_type& result, char t, const long long* val)
{
    if (val)
    {
        switch (t)
        {
        case 'm':
            write(result, *val * 1000);
            return;
        case 't':
            write_time(result, *val);
            return;
        case 'u':
            write_unixtime(result, *val);
            return;
        default:
            write(result, *val);
        }
    }
    else
        write_null(result);
}


void write_value(string_type& result, const char* val, std::size_t vs,
                 const char* key, std::size_t ks)
{
    if ((ks > 2) && (key[ks - 2] == '.'))
        write_value(result, key[ks - 1], val, vs);
    else
        write_value(result, '\0', val, vs);
}

void write_value(string_type& result, const double* val,
                 const char* key, std::size_t ks)
{
    if ((ks > 2) && (key[ks - 2] == '.'))
        write_value(result, key[ks - 1], val);
    else
        write_value(result, '\0', val);
}

void write_value(string_type& result, const long long* val,
                 const char* key, std::size_t ks)
{
    if ((ks > 2) && (key[ks - 2] == '.'))
        write_value(result, key[ks - 1], val);
    else
        write_value(result, '\0', val);
}

static inline void write_key(string_type& result,
    const char *key, std::size_t ks)
{
    write_unescape(result, key, ks);
    result += ':';
}

void write_key_value(string_type& result, const char *val, std::size_t vs,
    const char *key, std::size_t ks)
{
    if ((ks > 2) && (key[ks - 2] == '.'))
    {
        write_key(result, key, ks - 2);
        write_value(result, key[ks - 1], val, vs);
    }
    else
    {
        write_key(result, key, ks);
        write_value(result, '\0', val, vs);
    }
}

void write_key_value(string_type& result, const double *val,
    const char *key, std::size_t ks)
{
    if ((ks > 2) && (key[ks - 2] == '.'))
    {
        write_key(result, key, ks - 2);
        write_value(result, key[ks - 1], val);
    }
    else
    {
        write_key(result, key, ks);
        write_value(result, '\0', val);
    }
}

void write_key_value(string_type& result, const long long *val,
    const char *key, std::size_t ks)
{
    if ((ks > 2) && (key[ks - 2] == '.'))
    {
        write_key(result, key, ks - 2);
        write_value(result, key[ks - 1], val);
    }
    else
    {
        write_key(result, key, ks);
        write_value(result, '\0', val);
    }
}

} // namespace captor
