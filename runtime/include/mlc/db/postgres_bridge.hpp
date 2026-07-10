#pragma once

// Soft include: core codegen may reference this without requiring libpq.
// Call sites need libpq-dev (see mlc/db/postgres.hpp).

#if __has_include(<libpq-fe.h>) || __has_include(<postgresql/libpq-fe.h>)
#include "mlc/db/postgres.hpp"
#endif
