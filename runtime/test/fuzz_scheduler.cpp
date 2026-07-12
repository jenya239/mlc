// TestScheduler seed fuzz (TRACK_CONCURRENCY_TEST_HARNESS T6).
// Usage:
//   fuzz_scheduler <seed>
//   fuzz_scheduler --random <count> [seed0]
// Env: MLC_SCHEDULER_SEED (single-seed mode if argv omitted).
// g++ -std=c++20 -pthread -I../include -o fuzz_scheduler fuzz_scheduler.cpp

#include "mlc/concurrency/testing/channel.hpp"
#include "mlc/concurrency/testing/mutex.hpp"
#include "mlc/concurrency/testing/scheduler.hpp"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

[[noreturn]] void fail_seed(uint64_t seed, const std::string& reason) {
  std::cerr << "FAIL seed=" << seed << " " << reason << "\n";
  std::exit(1);
}

std::vector<std::string> run_mutex_scenario(uint64_t seed, int* increments_out) {
  mlc::concurrency::testing::TestScheduler scheduler(seed);
  mlc::concurrency::testing::TestMutex<int> guarded(scheduler, 0);
  std::atomic<int> increments{0};

  scheduler.spawn([&] {
    guarded.lock([&](int& counter) {
      scheduler.log_event("a_enter");
      counter = counter + 1;
      increments.fetch_add(1);
      scheduler.log_event("a_inc");
    });
    scheduler.log_event("a_done");
  });
  scheduler.spawn([&] {
    guarded.lock([&](int& counter) {
      scheduler.log_event("b_enter");
      counter = counter + 1;
      increments.fetch_add(1);
      scheduler.log_event("b_inc");
    });
    scheduler.log_event("b_done");
  });
  scheduler.spawn([&] {
    guarded.lock([&](int& counter) {
      scheduler.log_event("c_enter");
      counter = counter + 1;
      increments.fetch_add(1);
      scheduler.log_event("c_inc");
    });
    scheduler.log_event("c_done");
  });
  scheduler.join();
  *increments_out = increments.load();
  return scheduler.events();
}

std::vector<std::string> run_channel_scenario(uint64_t seed) {
  mlc::concurrency::testing::TestScheduler scheduler(seed);
  mlc::concurrency::testing::TestChannel<int> channel(scheduler, 1);

  scheduler.spawn([&] {
    if (!channel.send(7)) {
      throw std::runtime_error("send failed");
    }
    if (!channel.send(11)) {
      throw std::runtime_error("send failed");
    }
    scheduler.log_event("sent");
  });
  scheduler.spawn([&] {
    auto first = channel.receive();
    auto second = channel.receive();
    if (!first.has_value() || *first != 7) {
      throw std::runtime_error("bad first receive");
    }
    if (!second.has_value() || *second != 11) {
      throw std::runtime_error("bad second receive");
    }
    scheduler.log_event("received");
  });
  scheduler.join();
  return scheduler.events();
}

void run_seed(uint64_t seed) {
  int increments = 0;
  std::vector<std::string> first_mutex;
  std::vector<std::string> second_mutex;
  try {
    first_mutex = run_mutex_scenario(seed, &increments);
  } catch (const std::exception& error) {
    fail_seed(seed, std::string("mutex: ") + error.what());
  }
  if (increments != 3) {
    fail_seed(seed, "mutex increments != 3");
  }
  try {
    int ignored = 0;
    second_mutex = run_mutex_scenario(seed, &ignored);
  } catch (const std::exception& error) {
    fail_seed(seed, std::string("mutex replay: ") + error.what());
  }
  if (first_mutex != second_mutex) {
    fail_seed(seed, "mutex event log not deterministic");
  }
  if (first_mutex.empty()) {
    fail_seed(seed, "mutex event log empty");
  }

  std::vector<std::string> first_channel;
  std::vector<std::string> second_channel;
  try {
    first_channel = run_channel_scenario(seed);
  } catch (const std::exception& error) {
    fail_seed(seed, std::string("channel: ") + error.what());
  }
  try {
    second_channel = run_channel_scenario(seed);
  } catch (const std::exception& error) {
    fail_seed(seed, std::string("channel replay: ") + error.what());
  }
  if (first_channel != second_channel) {
    fail_seed(seed, "channel event log not deterministic");
  }
  if (first_channel.size() != 2) {
    fail_seed(seed, "channel event log size != 2");
  }

  std::cout << "OK seed=" << seed << "\n";
}

void print_usage() {
  std::cerr << "usage: fuzz_scheduler <seed>\n";
  std::cerr << "       fuzz_scheduler --random <count> [seed0]\n";
  std::cerr << "   or: MLC_SCHEDULER_SEED=<seed> fuzz_scheduler\n";
}

} // namespace

int main(int argc, char** argv) {
  if (argc >= 2 && std::string(argv[1]) == "--random") {
    if (argc < 3) {
      print_usage();
      return 2;
    }
    const uint64_t count = static_cast<uint64_t>(std::strtoull(argv[2], nullptr, 10));
    uint64_t seed0 = 0;
    if (argc >= 4) {
      seed0 = static_cast<uint64_t>(std::strtoull(argv[3], nullptr, 10));
    } else {
      seed0 = static_cast<uint64_t>(
          std::chrono::steady_clock::now().time_since_epoch().count()
      );
    }
    std::mt19937_64 generator(seed0);
    std::cout << "random count=" << count << " seed0=" << seed0 << "\n";
    for (uint64_t index = 0; index < count; ++index) {
      const uint64_t seed = generator();
      run_seed(seed);
    }
    return 0;
  }

  uint64_t seed = 0;
  if (argc >= 2) {
    seed = static_cast<uint64_t>(std::strtoull(argv[1], nullptr, 10));
  } else {
    const char* from_env = std::getenv("MLC_SCHEDULER_SEED");
    if (from_env == nullptr || from_env[0] == '\0') {
      print_usage();
      return 2;
    }
    seed = static_cast<uint64_t>(std::strtoull(from_env, nullptr, 10));
  }
  run_seed(seed);
  return 0;
}
