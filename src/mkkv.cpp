#include "journal.hpp"

#include <cstring>
#include <cassert>
#include <exception>
#include <cstdio>
#include <string>

#include "mysql.hpp"

// сделать ключ=значение длинной до 254 байт
extern "C" my_bool mkkv_init(UDF_INIT *initid,
    UDF_ARGS *args, char *msg)
{
    try
    {
#ifdef USE_COLUMN_NAME
        if (args->arg_count != 1)
        {
            strncpy(msg, "mkkv( <param> )", MYSQL_ERRMSG_SIZE);

            cout([]{
                static const auto text = "mkkv_init: arg_count != 1";
                return text;
            });

            return 1;
        }

        if (!args->attributes[0] || (args->attribute_lengths[0] == 0))
        {
            static const auto text = "mkkv_init: no key";
            strncpy(msg, text, MYSQL_ERRMSG_SIZE);
            cout([]{
                return text;
            });

            return 1;
        }
#else
    if (args->arg_count != 2)
    {
        strncpy(msg, "mkkv( <param> )", MYSQL_ERRMSG_SIZE);

        cout([]{
            static const auto text = "mkkv_init: arg_count != 2";
            return text;
        });

        return 1;
    }

    if (!args->args[0] || args->lengths[0] == 0)
    {
        static const auto text = "mkkv_init: no key";
        strncpy(msg, text, MYSQL_ERRMSG_SIZE);
        cout([]{
            return text;
        });

        return 1;
    }

    if (args->arg_type[0] != STRING_RESULT)
    {
        static const auto text = "mkkv_init: key is not string";
        strncpy(msg, text, MYSQL_ERRMSG_SIZE);
        cout([]{
            return text;
        });

        return 1;
    }
#endif
        initid->maybe_null = 1;
        initid->const_item = 0;

        return 0;
    }
    catch (const std::exception& e)
    {
        snprintf(msg, MYSQL_ERRMSG_SIZE, "%s", e.what());

        cerr([&]{
            std::string text("mkkv_init: ");
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = "mkkv_init :*(";
        strncpy(msg, text, MYSQL_ERRMSG_SIZE);
        cerr([&]{
            return text;
        });
    }

    return 1;
}

extern "C" char* mkkv(UDF_INIT* /* initid */, UDF_ARGS* args,
        char* result, unsigned long* length,
        char* /* is_null */, char* error)
{
    try
    {
#ifdef USE_COLUMN_NAME        
        // ключ должен быть
        auto k = args->attributes[0];
        auto type = args->arg_type[0];
        auto v = args->args[0];
        if (!v)
        {
            static char empty[] = "null";
            v = empty;
            type = STRING_RESULT;
        }
#else
        // ключ должен быть
        auto k = args->args[0];
        auto type = args->arg_type[1];
        auto v = args->args[1];
        if (!v)
        {
            static char empty[] = "null";
            v = empty;
            type = STRING_RESULT;
        }
#endif

        switch (type)
        {
        case REAL_RESULT:
            *length = static_cast<unsigned long>(
                snprintf(result, 255, "%s=%f", k,
                    *reinterpret_cast<double*>(args->args[0])));
            break;
        case INT_RESULT:
            *length = static_cast<unsigned long>(
                snprintf(result, 255, "%s=%lld", k,
                    *reinterpret_cast<long long*>(args->args[0])));
            break;
        case STRING_RESULT:
        case DECIMAL_RESULT:
            *length = static_cast<unsigned long>(
                snprintf(result, 255, "%s=%s", k, v));
            break;
        default:;
        }
        return result;
    }
    catch (const std::exception& e)
    {
        *length = static_cast<unsigned long>(
            snprintf(result, 255, "%s", e.what()));

        cerr([&]{
            std::string text("jst: ");
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = "mkkv :*(";
        *length = static_cast<unsigned long>(
            snprintf(result, 255, "%s", text));

        cerr([&]{
            return text;
        });
    }

    *error = 1;
    return result;
}

extern "C" void mkkv_deinit(UDF_INIT*)
{   }
