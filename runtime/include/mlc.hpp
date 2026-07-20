#pragma once

// MLC Runtime Library
// Main header that includes all runtime modules

// Standard C++ headers
#include <cmath>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

// Core types
#include "mlc/core/symbol.hpp"
#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"
#include "mlc/core/hashmap.hpp"
#include "mlc/core/collections.hpp"
#include "mlc/core/match.hpp"
#include "mlc/core/option.hpp"
#include "mlc/core/optional_combinators.hpp"
#include "mlc/core/result.hpp"
#include "mlc/core/result_combinators.hpp"
#include "mlc/core/task.hpp"
#include "mlc/core/profile.hpp"
#include "mlc/concurrency/channel.hpp"
#include "mlc/concurrency/spawn.hpp"
#include "mlc/concurrency/arc.hpp"
#include "mlc/concurrency/mutex.hpp"
#include "mlc/concurrency/atomic.hpp"
#include "mlc/concurrency/stop.hpp"
#include "mlc/concurrency/task_scope.hpp"
#include "mlc/concurrency/thread_pool.hpp"
#include "mlc/concurrency/job_queue.hpp"
#include "mlc/concurrency/isolate.hpp"
#include "mlc/concurrency/supervisor.hpp"
#include "mlc/memory/region.hpp"

// I/O
#include "mlc/io/io.hpp"
#include "mlc/core/arith.hpp"
#include "mlc/core/int_arith.hpp"
#include "mlc/io/file.hpp"
#include "mlc/io/buffer.hpp"

// Env (thin getenv FFI; Log is pure MLC — no C++ header)
#include "mlc/env/env_abi.hpp"

// Validation is pure MLC (validate.hpp deleted)
// Text processing
#include "mlc/text/regex.hpp"

// Math
#include "mlc/math/math.hpp"

// Graphics
#include "mlc/graphics/graphics.hpp"

// JSON
#include "mlc/json/json.hpp"

// Net
#include "mlc/net/tcp_bridge.hpp"
#include "mlc/net/websocket_bridge.hpp"

// DB (optional — requires libpq-fe.h; API in postgres_bridge.hpp)
#include "mlc/db/postgres_bridge.hpp"

// Crypto (optional — requires sodium.h; API in sodium_bridge.hpp)
#include "mlc/crypto/sodium_bridge.hpp"
