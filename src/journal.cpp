#include "journal.hpp"

#include <syslog.h>
#include <cassert>

using namespace captor;

journal::journal() noexcept
    : mask_(LOG_UPTO(LOG_NOTICE))
{
    openlog("capjs", LOG_ODELAY, LOG_USER);
    // нет смысла делать setlogmask
    // тк проверку уровня лога делаем мы сами
}

journal::~journal() noexcept
{
    closelog();
}

void journal::output(int level, const char *str) const noexcept
{
    assert(str);
    // %s из-за ворнинга
    syslog(level, "%s", str);
}

int journal::error_level() const noexcept
{
    return LOG_MASK(LOG_ERR);
}

int journal::notice_level() const noexcept
{
    return LOG_MASK(LOG_NOTICE);
}

bool journal::level_allow(int level) const noexcept
{
    return (mask_ & level) != 0;
}

static journal j;
