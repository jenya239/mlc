#include "mlc/reactor/https_transfer.hpp"

#include <chrono>
#include <cstdlib>
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

int read_counter(const std::string& name) {
    return std::stoi(read_text(directory_path() + "/" + name));
}

std::string authority_path() { return read_text(directory_path() + "/certificate_authority_path"); }

int fail(int code, const std::string& message) {
    std::cerr << message << "\n";
    return code;
}

mlc::Task<mlc::reactor::HttpsReactorResult> start_get(
    const std::string& url,
    std::int32_t timeout_milliseconds,
    mlc::concurrency::StopToken stop_token) {
    return mlc::reactor::start_https_transfer(
        mlc::String("GET"),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(""),
        timeout_milliseconds,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""),
        std::move(stop_token));
}

bool stopped(const mlc::reactor::HttpsReactorResult& result) {
    return result.failure_code == 7 && result.message == "stop requested";
}

int pre_cancel_does_not_connect() {
    const int connection_count_before = read_counter("connection_count");
    mlc::concurrency::StopSource stop_source;
    stop_source.request();
    const std::string url =
        "https://127.0.0.1:" + read_text(directory_path() + "/good_port") + "/echo";
    mlc::Task<mlc::reactor::HttpsReactorResult> task = start_get(url, 5000, stop_source.token());
    const mlc::reactor::HttpsReactorResult result = task.block_on();
    const int connection_count_delta = read_counter("connection_count") - connection_count_before;
    if (!stopped(result) || connection_count_delta != 0) {
        return fail(
            2,
            "pre_cancel connection_count_delta=" + std::to_string(connection_count_delta) +
                " failure=" + std::to_string(result.failure_code) + " message=" + result.message);
    }
    std::cout << "pre_cancel connection_count_delta=0 failure=7\n";
    return 0;
}

int request_stop_after_loop_destroyed() {
    mlc::concurrency::StopSource stop_source;
    std::promise<mlc::concurrency::StopSubscription> subscription_ready;
    std::promise<void> release_worker;
    std::thread worker([&] {
        subscription_ready.set_value(mlc::reactor::EventLoop::current().subscribe_stop(stop_source.token()));
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
    std::cout << "wakeup_after_loop_destroyed write_skipped=yes\n";
    return 0;
}

int cancel_silent_transfer() {
    const int connection_count_before = read_counter("connection_count");
    const std::string url =
        "https://127.0.0.1:" + read_text(directory_path() + "/silent_port") + "/silent";
    mlc::concurrency::StopSource stop_source;
    mlc::Task<mlc::reactor::HttpsReactorResult> task = start_get(url, 8000, stop_source.token());
    std::thread canceller([&] {
        for (int attempt = 0; attempt < 500; ++attempt) {
            if (read_counter("connection_count") > connection_count_before) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        stop_source.request();
    });
    const auto started = std::chrono::steady_clock::now();
    const mlc::reactor::HttpsReactorResult result = task.block_on();
    canceller.join();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - started);
    const int connection_count_delta = read_counter("connection_count") - connection_count_before;
    if (connection_count_delta < 1 || !stopped(result) || elapsed.count() >= 2000) {
        return fail(
            4,
            "silent_cancel elapsed_milliseconds=" + std::to_string(elapsed.count()) +
                " connection_count_delta=" + std::to_string(connection_count_delta) +
                " failure=" + std::to_string(result.failure_code) + " message=" + result.message);
    }
    std::cout << "silent_cancel elapsed_milliseconds=" << elapsed.count() << " failure=7\n";
    return 0;
}

}  // namespace

int main() {
    if (directory_path().empty() || authority_path().empty()) {
        return fail(1, "REACTOR_HTTPS_DIR is empty");
    }
    const int pre_cancel = pre_cancel_does_not_connect();
    if (pre_cancel != 0) return pre_cancel;
    const int destroyed = request_stop_after_loop_destroyed();
    if (destroyed != 0) return destroyed;
    return cancel_silent_transfer();
}
