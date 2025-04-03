#pragma once
#include <utility>
#include <string>

namespace captor {

class journal
{
    int mask_{};

public:
    journal() noexcept;

    ~journal() noexcept;

    template<class F>
    void cerr(F fn) const noexcept
    {
        try
        {
            auto level = error_level();
            if (level_allow(level))
                output(level, fn());
        }
        catch (...)
        {   }
    }

    template<class F>
    void cout(F fn) const noexcept
    {
        try
        {
            auto level = notice_level();
            if (level_allow(level))
                output(level, fn());
        }
        catch (...)
        {   }
    }

private:
    void output(int level, const char *str) const noexcept;

    template<class T>
    void output(int level, const std::basic_string<T>& text) const noexcept
    {
        output(level, text.c_str());
    }

    int error_level() const noexcept;

    int notice_level() const noexcept;

    bool level_allow(int level) const noexcept;
};

static inline journal& j() noexcept
{
    static journal j;
    return j;
}

} // namespace captor

template<class F>
static void cerr(F fn) noexcept
{
    captor::j().cerr(std::move(fn));
}

template<class F>
static void cout(F fn) noexcept
{
    captor::j().cout(std::move(fn));
}
