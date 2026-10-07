#include "mlc/db/sqlite_abi.hpp"

#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <thread>
#include <unistd.h>
#include <stdlib.h>

namespace {

int failure_count = 0;

void fail(const char* name) {
  std::fprintf(stderr, "FAIL %s code=%d extended=%d message=%s\n",
               name,
               mlc::db::sqlite_last_error_code_i(),
               mlc::db::sqlite_last_error_extended_i(),
               mlc::db::sqlite_last_error_message_s().c_str());
  failure_count += 1;
}

void expect(bool condition, const char* name) {
  if (!condition) {
    fail(name);
  }
}

bool error_recorded(int code) {
  if (mlc::db::sqlite_last_error_code_i() != code) {
    return false;
  }
  if (mlc::db::sqlite_last_error_code_i() == 0) {
    return false;
  }
  return mlc::db::sqlite_last_error_message_s().byte_size() > 0;
}

std::string make_temp_path() {
  char pattern[] = "/tmp/mlc_sqlite_abi_XXXXXX";
  const int descriptor = mkstemp(pattern);
  if (descriptor < 0) {
    return "";
  }
  close(descriptor);
  return std::string(pattern);
}

void remove_database(const std::string& path) {
  if (path.empty()) {
    return;
  }
  unlink(path.c_str());
  unlink((path + "-journal").c_str());
  unlink((path + "-wal").c_str());
  unlink((path + "-shm").c_str());
}

void check_memory_statements() {
  const std::int32_t connection = mlc::db::sqlite_open_i(mlc::String(":memory:"), 0);
  expect(connection > 0, "open memory");
  expect(mlc::db::sqlite_exec_i(connection, mlc::String("CREATE TABLE item(name TEXT UNIQUE)")) == 0,
         "create table");
  expect(mlc::db::sqlite_exec_i(connection, mlc::String("INSERT INTO item(name) VALUES ('a')")) == 0,
         "insert row");
  expect(mlc::db::sqlite_changes_i(connection) == 1, "changes is 1");
  expect(mlc::db::sqlite_last_error_code_i() == 0, "changes leaves no error");

  const int syntax = mlc::db::sqlite_exec_i(connection, mlc::String("NOT SQL"));
  expect(syntax == 1 && error_recorded(1), "syntax error code 1");

  const int unique = mlc::db::sqlite_exec_i(connection, mlc::String("INSERT INTO item(name) VALUES ('a')"));
  expect(unique == 19 && error_recorded(19), "unique code 19");
  expect(mlc::db::sqlite_last_error_extended_i() == 2067, "unique extended 2067");

  expect(mlc::db::sqlite_prepare_i(connection, mlc::String("SELECT 1; SELECT 2")) == 0, "trailing prepare handle");
  expect(error_recorded(21), "trailing SQL code 21");
  expect(std::strstr(mlc::db::sqlite_last_error_message_s().c_str(), "trailing SQL") != nullptr,
         "trailing SQL message");

  expect(mlc::db::sqlite_prepare_i(connection, mlc::String("")) == 0, "empty prepare");
  expect(error_recorded(21), "empty SQL code 21");
  expect(mlc::db::sqlite_prepare_i(connection, mlc::String("-- comment")) == 0, "comment prepare");
  expect(error_recorded(21), "comment SQL code 21");

  const std::int32_t statement = mlc::db::sqlite_prepare_i(connection, mlc::String("SELECT ?1"));
  expect(statement > 0, "prepare select");
  expect(mlc::db::sqlite_column_type_i(statement, 0) < 0, "column before step");
  expect(error_recorded(21), "no current row");
  expect(mlc::db::sqlite_bind_null_i(statement, 0) == 25, "bind index 0 is range");
  expect(error_recorded(25), "range message");

  const mlc::String embedded(std::string("a\0b", 3));
  expect(mlc::db::sqlite_bind_text_i(statement, 1, embedded) == 0, "bind text with nul");
  expect(mlc::db::sqlite_step_i(statement) == 100, "step bound text");
  const mlc::String text = mlc::db::sqlite_column_text_s(statement, 0);
  expect(text.byte_size() == 1 && text.c_str()[0] == 'a', "nul cuts text");
  expect(mlc::db::sqlite_reset_i(statement) == 0, "reset select");

  expect(mlc::db::sqlite_bind_blob_hex_i(statement, 1, mlc::String("abc")) == 21, "odd hex");
  expect(error_recorded(21), "odd hex message");
  expect(mlc::db::sqlite_bind_blob_hex_i(statement, 1, mlc::String("zz")) == 21, "bad hex");
  expect(mlc::db::sqlite_bind_f64_i(statement, 1, std::numeric_limits<double>::quiet_NaN()) == 0, "bind nan");
  expect(mlc::db::sqlite_step_i(statement) == 100, "step nan");
  expect(mlc::db::sqlite_column_type_i(statement, 0) == 5, "nan stored as null");
  expect(mlc::db::sqlite_reset_i(statement) == 0, "reset nan");

  expect(mlc::db::sqlite_bind_blob_hex_i(statement, 1, mlc::String("")) == 0, "bind empty blob");
  expect(mlc::db::sqlite_step_i(statement) == 100, "step empty blob");
  expect(mlc::db::sqlite_column_type_i(statement, 0) == 4, "empty blob type");
  expect(mlc::db::sqlite_column_blob_hex_s(statement, 0).byte_size() == 0, "empty blob text");
  expect(mlc::db::sqlite_reset_i(statement) == 0, "reset empty blob");

  expect(mlc::db::sqlite_bind_blob_hex_i(statement, 1, mlc::String("AB")) == 0, "bind upper hex");
  expect(mlc::db::sqlite_step_i(statement) == 100, "step upper hex");
  const mlc::String upper = mlc::db::sqlite_column_blob_hex_s(statement, 0);
  expect(upper.byte_size() == 2 && upper.c_str()[0] == 'a' && upper.c_str()[1] == 'b', "upper hex lower result");
  expect(mlc::db::sqlite_reset_i(statement) == 0, "reset upper hex");
  expect(mlc::db::sqlite_bind_blob_hex_i(statement, 1, mlc::String("ab")) == 0, "bind lower hex");
  expect(mlc::db::sqlite_step_i(statement) == 100, "step lower hex");
  const mlc::String lower = mlc::db::sqlite_column_blob_hex_s(statement, 0);
  expect(lower.byte_size() == 2 && std::memcmp(lower.raw_data(), upper.raw_data(), 2) == 0, "hex case matches");
  expect(mlc::db::sqlite_finalize_i(statement) == 0, "finalize select");

  const std::int32_t insert = mlc::db::sqlite_prepare_i(connection, mlc::String("INSERT INTO item(name) VALUES (?1)"));
  expect(insert > 0, "prepare insert");
  expect(mlc::db::sqlite_bind_text_i(insert, 1, mlc::String("b")) == 0, "bind first name");
  expect(mlc::db::sqlite_step_i(insert) == 101, "insert step done");
  expect(mlc::db::sqlite_reset_i(insert) == 0, "reset insert");
  expect(mlc::db::sqlite_bind_text_i(insert, 1, mlc::String("b")) == 0, "bind duplicate name");
  expect(mlc::db::sqlite_step_i(insert) == 19, "duplicate step code 19");
  expect(mlc::db::sqlite_reset_i(insert) == 0, "reset swallows duplicate");
  expect(mlc::db::sqlite_last_error_code_i() == 0, "reset is not a second error");

  expect(mlc::db::sqlite_close_i(connection) == 0, "close with live statement");
  expect(mlc::db::sqlite_finalize_i(insert) == 21, "finalize after close");
  expect(error_recorded(21), "finalize after close message");
  expect(mlc::db::sqlite_close_i(connection) == 21, "second close");
  expect(error_recorded(21), "second close message");

  const std::int32_t reopened = mlc::db::sqlite_open_i(mlc::String(":memory:"), 0);
  expect(reopened > connection, "handle numbers increase");
  expect(mlc::db::sqlite_close_i(reopened) == 0, "close reopened");
}

void check_other_thread(std::int32_t connection_handle, int* result_code) {
  *result_code = mlc::db::sqlite_exec_i(connection_handle, mlc::String("SELECT 1"));
}

void check_busy_commit(const std::string& path) {
  const std::int32_t first = mlc::db::sqlite_open_i(mlc::String(path), 0);
  const std::int32_t second = mlc::db::sqlite_open_i(mlc::String(path), 0);
  expect(first > 0 && second > 0, "busy connections");
  expect(mlc::db::sqlite_exec_i(first, mlc::String("CREATE TABLE IF NOT EXISTS item(name TEXT)")) == 0,
         "busy create");
  expect(mlc::db::sqlite_exec_i(first, mlc::String("BEGIN IMMEDIATE")) == 0, "first immediate begin");
  expect(mlc::db::sqlite_busy_timeout_i(second, 0) == 0, "second timeout 0");
  const int blocked_begin = mlc::db::sqlite_exec_i(second, mlc::String("BEGIN IMMEDIATE"));
  expect(blocked_begin == 5 && error_recorded(5), "second immediate is busy");
  expect(mlc::db::sqlite_autocommit_i(second) == 1, "failed immediate starts no transaction");
  expect(mlc::db::sqlite_exec_i(first, mlc::String("ROLLBACK")) == 0, "release first immediate");

  expect(mlc::db::sqlite_exec_i(first, mlc::String("BEGIN")) == 0, "first deferred begin");
  expect(mlc::db::sqlite_exec_i(first, mlc::String("SELECT COUNT(*) FROM item")) == 0, "first shared read");
  expect(mlc::db::sqlite_exec_i(second, mlc::String("BEGIN IMMEDIATE")) == 0, "second immediate while shared");
  expect(mlc::db::sqlite_exec_i(second, mlc::String("INSERT INTO item(name) VALUES ('x')")) == 0,
         "second insert under shared lock");
  const int blocked_commit = mlc::db::sqlite_exec_i(second, mlc::String("COMMIT"));
  expect(blocked_commit == 5 && error_recorded(5), "commit is busy");
  expect(mlc::db::sqlite_autocommit_i(second) == 0, "busy commit leaves the transaction open");
  expect(mlc::db::sqlite_exec_i(second, mlc::String("ROLLBACK")) == 0, "second rollback");
  expect(mlc::db::sqlite_autocommit_i(second) == 1, "rollback leaves autocommit");
  expect(mlc::db::sqlite_exec_i(first, mlc::String("COMMIT")) == 0, "first commit");
  expect(mlc::db::sqlite_close_i(first) == 0, "close busy first");
  expect(mlc::db::sqlite_close_i(second) == 0, "close busy second");
}

void check_file_and_thread() {
  const std::string path = make_temp_path();
  expect(!path.empty(), "temp path");
  const std::int32_t connection = mlc::db::sqlite_open_i(mlc::String(path), 0);
  expect(connection > 0, "open file");
  expect(mlc::db::sqlite_exec_i(connection, mlc::String("CREATE TABLE item(name TEXT)")) == 0, "file create");
  expect(mlc::db::sqlite_close_i(connection) == 0, "close file");

  const std::string missing = path + "_missing";
  const std::int32_t readonly = mlc::db::sqlite_open_i(mlc::String(missing), 1);
  expect(readonly == 0, "readonly missing handle");
  expect(error_recorded(14), "readonly missing is cantopen");

  const std::int32_t again = mlc::db::sqlite_open_i(mlc::String(path), 0);
  expect(again > connection, "file handle increases");
  int foreign = 0;
  std::thread worker(check_other_thread, again, &foreign);
  worker.join();
  expect(foreign == 21, "other thread is misuse");
  expect(mlc::db::sqlite_exec_i(again, mlc::String("SELECT COUNT(*) FROM item")) == 0, "owner thread still works");
  expect(mlc::db::sqlite_close_i(again) == 0, "close file again");
  check_busy_commit(path);
  remove_database(path);
}

}  // namespace

int main() {
  if (mlc::db::sqlite_libversion_number_i() < 3007015) {
    std::fprintf(stderr, "FAIL sqlite version %d\n", mlc::db::sqlite_libversion_number_i());
    return 1;
  }
  check_memory_statements();
  check_file_and_thread();
  if (failure_count != 0) {
    std::fprintf(stderr, "sqlite abi smoke failures=%d\n", failure_count);
    return 1;
  }
  std::printf("sqlite abi smoke OK version=%s\n", mlc::db::sqlite_libversion_s().c_str());
  return 0;
}
