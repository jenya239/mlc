#include "mlc/concurrency/sleep_abi.hpp"

#include <chrono>

int main() {
  if (mlc::sleep_abi::sleep_milliseconds(0) != 0) return 1;
  if (mlc::sleep_abi::sleep_milliseconds(-5) != 0) return 2;
  const auto started = std::chrono::steady_clock::now();
  if (mlc::sleep_abi::sleep_milliseconds(1) != 0) return 3;
  const auto elapsed = std::chrono::steady_clock::now() - started;
  if (elapsed < std::chrono::milliseconds(1)) return 4;
  if (elapsed > std::chrono::milliseconds(2000)) return 5;
  return 0;
}
