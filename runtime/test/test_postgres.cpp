// Postgres libpq smoke (TRACK_STDLIB_POSTGRES STEP=2).
// Prefer: scripts/run_postgres_runtime_smoke.sh

#include "mlc/db/postgres.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

static int passed = 0;
static int failed = 0;

#define CHECK(expression)                                                                          \
  do {                                                                                             \
    if (expression) {                                                                              \
      ++passed;                                                                                    \
    } else {                                                                                       \
      ++failed;                                                                                    \
      std::cerr << "FAIL: " #expression " at line " << __LINE__ << "\n";                            \
    }                                                                                              \
  } while (0)

void test_bad_connect() {
  auto handle = mlc::db::connect(mlc::String("host=127.0.0.1 port=1 dbname=mlc_missing user=mlc connect_timeout=1"));
  CHECK(!handle.has_value());
  CHECK(mlc::db::last_error().size() > 0);
}

void test_invalid_handles() {
  CHECK(!mlc::db::exec(0, mlc::String("SELECT 1")).has_value());
  CHECK(mlc::db::ntuples(0) == 0);
  CHECK(mlc::db::nfields(-1) == 0);
  CHECK(!mlc::db::getvalue(0, 0, 0).has_value());
  mlc::db::clear(0);
  mlc::db::finish(0);
  CHECK(true);
}

void test_live_select_one_if_env() {
  const char* database_url = std::getenv("DATABASE_URL");
  std::string conninfo;
  if (database_url != nullptr && database_url[0] != '\0') {
    conninfo = database_url;
  } else if (std::getenv("PGHOST") != nullptr) {
    conninfo = "dbname=";
    const char* database_name = std::getenv("PGDATABASE");
    conninfo += database_name != nullptr ? database_name : "postgres";
  } else {
    std::cout << "live SELECT 1: skipped (set DATABASE_URL or PGHOST)\n";
    return;
  }

  auto connection = mlc::db::connect(mlc::String(conninfo));
  CHECK(connection.has_value());
  if (!connection.has_value()) {
    std::cerr << "connect: " << mlc::db::last_error().c_str() << "\n";
    return;
  }
  auto result = mlc::db::exec(*connection, mlc::String("SELECT 1 AS one"));
  CHECK(result.has_value());
  if (!result.has_value()) {
    std::cerr << "exec: " << mlc::db::last_error().c_str() << "\n";
    mlc::db::finish(*connection);
    return;
  }
  CHECK(mlc::db::ntuples(*result) == 1);
  CHECK(mlc::db::nfields(*result) == 1);
  auto cell = mlc::db::getvalue(*result, 0, 0);
  CHECK(cell.has_value());
  if (cell.has_value()) {
    CHECK(cell->view() == "1");
  }
  mlc::db::clear(*result);
  mlc::db::finish(*connection);
}

int main() {
  test_bad_connect();
  test_invalid_handles();
  test_live_select_one_if_env();
  std::cout << "postgres smoke: " << passed << " passed, " << failed << " failed\n";
  return failed == 0 ? 0 : 1;
}
