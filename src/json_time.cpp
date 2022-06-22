#include "btdef/date.hpp"
#include "journal.hpp"

#include "mysql.hpp"

// конвертация в json
extern "C" my_bool jst_init(UDF_INIT* initid,
    UDF_ARGS* args, char* msg)
{
    try
    {
        if (args->arg_count != 1)
        {
            strncpy(msg, "use jst( <iso date> )", MYSQL_ERRMSG_SIZE);

            cout([]{
                return std::string("jst_init: iso date");
            });

            return 1;
        }
        auto t = args->arg_type[0];
        if (!(t == STRING_RESULT))
        {
            strncpy(msg, "bad arg type, use: jst(\"iso date\")",
                    MYSQL_ERRMSG_SIZE);

            cout([]{
                return std::string("jst_init: bad arg type");
            });

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

        cerr([&]{
            std::string text("jst_init: ");
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        static const std::string text("jsd_init :*(");

        strncpy(msg, text.c_str(), MYSQL_ERRMSG_SIZE);

        cerr([&]{
            return text;
        });
    }

    return 1;
}

extern "C" long long jst(UDF_INIT* /* initid */,
    UDF_ARGS* args, char* is_null, char* error)
{
    try
    {
        // тип проверили в init'emm
        auto v = args->args[0];
        if (v)
        {
            auto d = btdef::date::parse(v);
            return static_cast<long long>(d.time());
        }
    }
    catch (const std::exception& e)
    {
        cerr([&]{
            std::string text("jst: ");
            text += e.what();
            return text;
        });
    }
    catch (...)
    {
        cerr([&]{
            static const std::string text("jst :*(");
            return text;
        });
    }

    *is_null = 1;
    *error = 1;

    return 0;
}

extern "C" void jst_deinit(UDF_INIT*)
{   }
