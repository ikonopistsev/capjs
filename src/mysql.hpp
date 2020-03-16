#pragma once

#include "mysql.h"

#if MYSQL_VERSION_ID >= 80010
typedef bool my_bool;
#endif