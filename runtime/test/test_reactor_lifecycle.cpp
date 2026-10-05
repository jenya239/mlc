#include "mlc/reactor/https_transfer.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

namespace {

std::string directory_path() {
    const char* value = std::getenv("REACTOR_HTTPS_DIR");
    return value == nullptr ? std::string() : std::string(value);
}

std::string read_text(const std::string& path) {
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    std::string text = buffer.str();
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) {
        text.pop_back();
    }
    return text;
}

std::string authority_path() { return read_text(directory_path() + "/certificate_authority_path"); }

int fail(int code, const std::string& message) {
    std::cerr << message << "\n";
    return code;
}

int count_open_descriptors() {
    DIR* directory = ::opendir("/proc/self/fd");
    if (directory == nullptr) return -1;
    int count = 0;
    while (const dirent* entry = ::readdir(directory)) {
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) continue;
        ++count;
    }
    ::closedir(directory);
    return count;
}

int read_thread_count() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        const std::string prefix = "Threads:";
        if (line.rfind(prefix, 0) == 0) {
            return std::stoi(line.substr(prefix.size()));
        }
    }
    return -1;
}

int descriptor_lifetime() {
    const int descriptor_count_before = count_open_descriptors();
    if (descriptor_count_before < 0) {
        return fail(2, "descriptor count unreadable");
    }
    {
        std::thread worker([] { mlc::reactor::EventLoop::current(); });
        worker.join();
    }
    const int descriptor_count_after = count_open_descriptors();
    if (descriptor_count_after != descriptor_count_before) {
        return fail(
            2,
            "descriptor count changed " + std::to_string(descriptor_count_before) + " -> " +
                std::to_string(descriptor_count_after));
    }
    std::cout << "descriptor_count_unchanged count=" << descriptor_count_after << "\n";

    mlc::concurrency::StopSource stop_source;
    std::promise<mlc::concurrency::StopSubscription> subscription_ready;
    std::promise<void> release_worker;
    std::thread worker([&] {
        subscription_ready.set_value(
            mlc::reactor::EventLoop::current().subscribe_stop(stop_source.token()));
        release_worker.get_future().wait();
    });
    mlc::concurrency::StopSubscription subscription = subscription_ready.get_future().get();
    release_worker.set_value();
    worker.join();
    try {
        stop_source.request();
    } catch (const std::exception& error) {
        return fail(3, std::string("wakeup_after_loop_destroyed write failed: ") + error.what());
    }
    subscription.unsubscribe();
    const int descriptor_count_final = count_open_descriptors();
    if (descriptor_count_final != descriptor_count_before) {
        return fail(
            3,
            "descriptor count after wake " + std::to_string(descriptor_count_before) + " -> " +
                std::to_string(descriptor_count_final));
    }
    std::cout << "wakeup_after_loop_destroyed write_skipped=yes\n";
    return 0;
}

bool transfer_succeeded(const mlc::reactor::HttpsReactorResult& result) {
    return result.status == 200 && result.failure_code == 0;
}

int one_get(const std::string& url) {
    mlc::Task<mlc::reactor::HttpsReactorResult> task = mlc::reactor::start_https_transfer(
        mlc::String("GET"),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(""),
        5000,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""),
        mlc::concurrency::StopSource{}.token());
    const mlc::reactor::HttpsReactorResult result = task.block_on();
    if (!transfer_succeeded(result)) {
        return fail(
            4,
            "sequential status=" + std::to_string(result.status) +
                " failure=" + std::to_string(result.failure_code));
    }
    return 0;
}

int sequential_requests() {
    const std::string url = "https://127.0.0.1:" + read_text(directory_path() + "/good_port") + "/binary";
    for (int warmup = 0; warmup < 2; ++warmup) {
        const int warmup_code = one_get(url);
        if (warmup_code != 0) return warmup_code;
    }
    const int thread_count_before = read_thread_count();
    if (thread_count_before < 1) {
        return fail(5, "thread count unreadable before sequential requests");
    }
    for (int request_index = 0; request_index < 100; ++request_index) {
        const int request_code = one_get(url);
        if (request_code != 0) return request_code;
    }
    const int thread_count_after = read_thread_count();
    if (thread_count_after != thread_count_before) {
        return fail(
            5,
            "thread count changed " + std::to_string(thread_count_before) + " -> " +
                std::to_string(thread_count_after));
    }
    std::cout << "sequential_requests=100 thread_count_unchanged count=" << thread_count_after << "\n";
    return 0;
}

}  // namespace

int main() {
    const int descriptors = descriptor_lifetime();
    if (descriptors != 0) return descriptors;
    if (directory_path().empty() || authority_path().empty()) {
        return fail(1, "REACTOR_HTTPS_DIR is empty");
    }
    return sequential_requests();
}
