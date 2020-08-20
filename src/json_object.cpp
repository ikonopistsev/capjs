#include "json_object.hpp"
#include "btdef/util/basic_string_ext.hpp"
#include "btdef/ref/basic_string_ext.hpp"
#include "btdef/text.hpp"
#include "journal.hpp"
#include "mysql.hpp"

#include <algorithm>

namespace captor {

json_object::json_object(std::size_t capacity)
{
    text_.reserve(capacity);
}

void json_object::start()
{
    // обнуляем переменную и присваиваем [
    text_ = '{';
}

void json_object::end()
{
    text_ += '}';
}

void json_object::add(const char* key,
    std::size_t ks, const char* val, std::size_t vs)
{
    if (size() > 1)
        text_ += ',';

    write_key_value(text_, val, vs, key, ks);
}

void json_object::add(const char* key, std::size_t ks, const double* val)
{
    if (size() > 1)
        text_ += ',';

    write_key_value(text_, val, key, ks);
}

void json_object::add(const char* key, std::size_t ks, const long long* val)
{
    if (text_.size() > 1)
        text_ += ',';

    write_key_value(text_, val, key, ks);
}

} // namespace captor

// конвертация в json object
extern "C" my_bool jsobj_init(UDF_INIT* initid,
    UDF_ARGS* args, char* msg)
{
    try
    {
        auto arg_size = 0u;
        for (unsigned int i = 0; i < args->arg_count; ++i)
        {
            auto key = args->attributes[i];
            auto key_size = args->attribute_lengths[i];
            if (!key || (key_size == 0))
            {
                snprintf(msg, MYSQL_ERRMSG_SIZE, "empty key[%d] name", i);

                cout([&]{
                    auto text = std::mkstr(
                        std::cref("jsobj_init: key name empty - "));
                    text += btdef::to_text(i);
                    return text;
                });

                return 1;
            }
            arg_size += args->lengths[i] + key_size;
        }

#ifdef TRACE_CAPACITY
        auto need_size = arg_size;
#endif // TRACE_CAPACITY
        arg_size *= 2u;

        constexpr auto div = 256u;
        arg_size = ((arg_size / div) * div) + div;

#ifdef TRACE_CAPACITY
        cout([&]{
            auto text = std::mkstr(std::cref("reserve: "));
            text += btdef::to_text(arg_size);
            text += std::cref(" need: ");
            text += btdef::to_text(need_size);
            return text;
        });
#endif // TRACE_CAPACITY

        initid->maybe_null = 0;
        initid->ptr = reinterpret_cast<char*>(
            new captor::json_object(std::min(8192u, arg_size)));

        return 0;
    }
    catch (const std::exception& e)
    {
        snprintf(msg, MYSQL_ERRMSG_SIZE, "%s", e.what());

        cerr([&]{
            auto text = std::mkstr(std::cref("jsobj_init: "));
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = std::mkstr(std::cref("jsobj_init :*("));

        strncpy(msg, text.c_str(), MYSQL_ERRMSG_SIZE);

        cerr([&]{
            return text;
        });
    }

    return 1;
}

extern "C" char* jsobj(UDF_INIT* initid, UDF_ARGS* args,
        char* result, unsigned long* length,
        char* /* is_null */, char* error)
{
    try
    {
        auto j = reinterpret_cast<captor::json_object*>(initid->ptr);

        j->start();

        for (unsigned int i = 0; i < args->arg_count; ++i)
        {
            auto v = args->args[i];
            auto key = args->attributes[i];
            auto key_size = args->attribute_lengths[i];
            switch (args->arg_type[i])
            {
            case REAL_RESULT:
                j->add(key, key_size, reinterpret_cast<double*>(v));
                break;
            case INT_RESULT:
                j->add(key, key_size, reinterpret_cast<long long*>(v));
                break;
            case STRING_RESULT:
            case DECIMAL_RESULT:
                j->add(key, key_size, v, args->lengths[i]);
                break;
            default:;
            }
        }

        j->end();

#ifdef TRACE_CAPACITY
        cout([&]{
            auto text = std::mkstr(std::cref("size: "));
            text += btdef::to_text(j->size());
            text += std::mkstr(std::cref(" capacity: "));
            text += btdef::to_text(j->capacity());
            return text;

        });
#endif // TRACE_CAPACITY

        *length = j->size();
        return const_cast<char*>(j->data());
    }
    catch (const std::exception& e)
    {
        *length = static_cast<unsigned long>(
            snprintf(result, 255, "%s", e.what()));

        cerr([&]{
            auto text = std::mkstr(std::cref("jsobj: "));
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = std::mkstr(std::cref("jsobj :*("));

        *length = static_cast<unsigned long>(
            snprintf(result, 255, "%s", text.c_str()));

        cerr([&]{
            return text;
        });
    }

    *error = 1;
    return result;
}

extern "C" void jsobj_deinit(UDF_INIT* initid)
{
    try
    {
        delete reinterpret_cast<captor::json_object*>(initid->ptr);
    }
    catch (...)
    {   }
}

