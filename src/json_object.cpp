#include "json_object.hpp"
#include "journal.hpp"
#include "btdef/text.hpp"

#include <mysql.h>

namespace captor {

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
        for (unsigned int i = 0; i < args->arg_count; ++i)
        {
            auto key = args->attributes[i];
            auto key_size = args->attribute_lengths[i];
            if (!key || (key_size == 0))
            {
                snprintf(msg, MYSQL_ERRMSG_SIZE, "empty key[%d] name", i);

                captor::j.cout([&]{
                    auto text = std::mkstr(
                        std::cref("jsobj_init: key name empty - "));
                    text += btdef::to_text(i);
                    return text;
                });

                return 1;
            }
        }
        initid->maybe_null = 0;
        initid->ptr = reinterpret_cast<char*>(new captor::json_object());

        return 0;
    }
    catch (const std::exception& e)
    {
        snprintf(msg, MYSQL_ERRMSG_SIZE, "%s", e.what());

        captor::j.cerr([&]{
            auto text = std::mkstr(std::cref("jsobj_init: "));
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = std::mkstr(std::cref("jsobj_init :*("));

        strncpy(msg, text.c_str(), MYSQL_ERRMSG_SIZE);

        captor::j.cerr([&]{
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

        *length = j->size();
        return const_cast<char*>(j->data());
    }
    catch (const std::exception& e)
    {
        *length = static_cast<unsigned long>(
            snprintf(result, 255, "%s", e.what()));

        captor::j.cerr([&]{
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

        captor::j.cerr([&]{
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

