#include "btdef.hpp"

#include <mysql.h>

// конвертация в json
extern "C" my_bool jsd_init(UDF_INIT* initid,
    UDF_ARGS* args, char* msg)
{
    try
    {
        if (args->arg_count != 1)
        {
            strncpy(msg, "use jsd( <miliseconds> )", MYSQL_ERRMSG_SIZE);
            return 1;
        }

        initid->max_length= 25;
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
        strncpy(msg, "jsd_init :*(", MYSQL_ERRMSG_SIZE);
    }

    return 1;
}

extern "C" char* jsd(UDF_INIT* /* initid */, UDF_ARGS *args,
        char* result, unsigned long* length,
        char* /* is_null */, char* error)
{
    try
    {
        auto v = args->args[0];
        if (!v)
        {
            static const ref::string empty(std::cref("null"));
            static const auto sz = empty.size();
            std::memcpy(result, empty.data(), sz);
            *length = sz;
            return result;
        }

        utility::date::value_t t = 0;
        switch (args->arg_type[0])
        {
        case REAL_RESULT:
            t = static_cast<utility::date::value_t>(
                *reinterpret_cast<double*>(args->args[0]));
            break;
        case INT_RESULT:
            t = static_cast<utility::date::value_t>(
                *reinterpret_cast<long long*>(args->args[0]));
            break;
        case STRING_RESULT:
        case DECIMAL_RESULT:
            t = static_cast<utility::date::value_t>(std::atof(v));
            break;
        default:;
        }
        auto text = utility::date(t).json_text();
        auto size = text.size();
        std::memcpy(result, text.data(), size);
        result[size] = '\0';

        *length = size;
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
            snprintf(result, 255, "jsd :*("));
    }

    *error = 1;
    return result;
}

extern "C" void jsd_deinit(UDF_INIT*)
{   }
