#include "btdef/date.hpp"
#include "journal.hpp"

#include "mysql.hpp"

// конвертация в json
extern "C" my_bool jsd_init(UDF_INIT* initid,
    UDF_ARGS* args, char* msg)
{
    try
    {
        if (args->arg_count != 1)
        {
            static const auto text = "jsd_init: arg_count != 1";
            strncpy(msg, "use jsd( <miliseconds> )", MYSQL_ERRMSG_SIZE);
            cout([&]{
                return text;
            });

            return 1;
        }

        initid->max_length= 24;
        initid->maybe_null = 1;
        initid->const_item = 0;

        return 0;
    }
    catch (const std::exception& e)
    {
        snprintf(msg, MYSQL_ERRMSG_SIZE, "%s", e.what());

        cerr([&]{
            std::string text("jsd_init: ");
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = "jsd_init :*(";
        strncpy(msg, text, MYSQL_ERRMSG_SIZE);
        cerr([&]{
            return text;
        });
    }

    return 1;
}

extern "C" char* jsd(UDF_INIT*, UDF_ARGS *args,
        char* result, unsigned long* length,
        char* /* is_null */, char* error)
{
    try
    {
        auto v = args->args[0];
        if (!v)
        {
            static const std::string empty("null");
            static const auto sz = empty.size();
            std::memcpy(result, empty.data(), sz);
            *length = sz;
            return result;
        }

        btdef::date::value_t t = 0;
        switch (args->arg_type[0])
        {
        case REAL_RESULT:
            t = static_cast<btdef::date::value_t>(
                *reinterpret_cast<double*>(args->args[0]));
            break;
        case INT_RESULT:
            t = static_cast<btdef::date::value_t>(
                *reinterpret_cast<long long*>(args->args[0]));
            break;
        case STRING_RESULT:
        case DECIMAL_RESULT:
            t = static_cast<btdef::date::value_t>(std::atof(v));
            break;
        default:;
        }
        auto text = btdef::date(t).to_json();
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

        cerr([&]{
            std::string text("jsd: ");
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const auto text = "jsd :*(";
        *length = static_cast<unsigned long>(
            snprintf(result, 255, "%s", text));
        cerr([&]{
            return text;
        });
    }

    *error = 1;
    return result;
}

extern "C" void jsd_deinit(UDF_INIT*)
{   }
