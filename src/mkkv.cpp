#include <mysql.h>
#include <cstring>
#include <cassert>
#include <exception>
#include <cstdio>

// сделать ключ=значение длинной до 254 байт

extern "C" my_bool mkkv_init(UDF_INIT *initid,
    UDF_ARGS *args, char *msg)
{
    try
    {
        if (args->arg_count != 1)
        {
            strncpy(msg, "mkkv( <param> )", MYSQL_ERRMSG_SIZE);
            return 1;
        }

        if (!args->attributes[0] || (args->attribute_lengths[0] == 0))
        {
            strncpy(msg, "mkkv no key", MYSQL_ERRMSG_SIZE);
            return 1;
        }

        initid->maybe_null = 1;
        initid->const_item = 0;

        return 0;
    }
    catch (const std::exception& e)
    {
        snprintf(msg, MYSQL_ERRMSG_SIZE, "%s", e.what());
    }
    catch (...)
    {
        strncpy(msg, "mkkv_init :*(", MYSQL_ERRMSG_SIZE);
    }

    return 1;
}

extern "C" char* mkkv(UDF_INIT* /* initid */, UDF_ARGS* args,
        char* result, unsigned long* length,
        char* /* is_null */, char* error)
{
    try
    {
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
    }
    catch (...)
    {
        *length = static_cast<unsigned long>(
            snprintf(result, 255, "mkkv :*("));
    }

    *error = 1;
    return result;
}

extern "C" void mkkv_deinit(UDF_INIT*)
{   }
