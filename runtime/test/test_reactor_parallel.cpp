#include "mlc/net/curl_abi.hpp"
#include "mlc/reactor/https_transfer.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

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

std::string text_of(const mlc::String& text) {
    if (text.raw_size() == 0 || text.raw_data() == nullptr) return std::string();
    return std::string(text.raw_data(), text.raw_size());
}

std::string port_url(const std::string& port, const std::string& path) {
    return "https://127.0.0.1:" + port + path;
}

std::string authority_path() { return read_text(directory_path() + "/certificate_authority_path"); }

int read_counter(const std::string& name) {
    return std::stoi(read_text(directory_path() + "/" + name));
}

int read_thread_count() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        constexpr std::string_view prefix = "Threads:";
        if (line.rfind(prefix, 0) == 0) {
            return std::stoi(line.substr(prefix.size()));
        }
    }
    return -1;
}

int fail(int code, const std::string& message) {
    std::cerr << message << "\n";
    return code;
}

std::string port_from_location(const std::string& header_block) {
    const std::string marker = "127.0.0.1:";
    const auto start = header_block.find(marker);
    if (start == std::string::npos) return std::string();
    const auto digits = start + marker.size();
    std::size_t end = digits;
    while (end < header_block.size() && header_block[end] >= '0' && header_block[end] <= '9') {
        ++end;
    }
    if (end == digits) return std::string();
    return header_block.substr(digits, end - digits);
}

std::string read_cross_port() {
    const std::string url = port_url(read_text(directory_path() + "/good_port"), "/redirect/cross");
    const std::int32_t handle = mlc::curl_abi::perform_request(
        mlc::String("GET"),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(""),
        5000,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""));
    const std::int32_t status = mlc::curl_abi::result_status(handle);
    const std::string header_block = text_of(mlc::curl_abi::result_header_block(handle));
    mlc::curl_abi::release_result(handle);
    if (status != 302) return std::string();
    return port_from_location(header_block);
}

mlc::Task<mlc::reactor::HttpsReactorResult> start_get(
    const std::string& url, std::int32_t timeout_milliseconds) {
    return mlc::reactor::start_https_transfer(
        mlc::String("GET"),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(""),
        timeout_milliseconds,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""),
        mlc::concurrency::StopSource{}.token());
}

bool succeeded(const mlc::reactor::HttpsReactorResult& result) {
    return result.status == 200 && result.failure_code == 0;
}

}  // namespace

int main() {
    if (directory_path().empty()) {
        return fail(1, "REACTOR_HTTPS_DIR is empty");
    }

    const std::string good_port = read_text(directory_path() + "/good_port");
    const std::string cross_port = read_cross_port();
    const std::string silent_port = read_text(directory_path() + "/silent_port");
    if (good_port.empty() || cross_port.empty() || silent_port.empty()) {
        return fail(1, "missing good_port, cross_port, or silent_port");
    }
    if (good_port == cross_port) {
        return fail(1, "cross_port matches good_port");
    }

    const int thread_count_before = read_thread_count();
    const int connection_count_before = read_counter("connection_count");
    if (thread_count_before < 1) {
        return fail(2, "thread count unreadable before wait");
    }

    mlc::Task<mlc::reactor::HttpsReactorResult> first =
        start_get(port_url(good_port, "/binary"), 5000);
    mlc::Task<mlc::reactor::HttpsReactorResult> second =
        start_get(port_url(cross_port, "/binary"), 5000);
    while (!first.is_ready() || !second.is_ready()) {
        if (!first.is_ready()) {
            first.block_on();
        } else {
            second.block_on();
        }
    }
    const mlc::reactor::HttpsReactorResult first_result = first.block_on();
    const mlc::reactor::HttpsReactorResult second_result = second.block_on();
    if (!succeeded(first_result) || !succeeded(second_result)) {
        return fail(
            3,
            "parallel status " + std::to_string(first_result.status) + "/" +
                std::to_string(second_result.status) + " failure " +
                std::to_string(first_result.failure_code) + "/" +
                std::to_string(second_result.failure_code));
    }

    const int connection_count_delta = read_counter("connection_count") - connection_count_before;
    const int thread_count_after = read_thread_count();
    if (connection_count_delta != 2) {
        return fail(4, "connection_count_delta=" + std::to_string(connection_count_delta));
    }
    if (thread_count_after != thread_count_before) {
        return fail(
            4,
            "thread count changed " + std::to_string(thread_count_before) + " -> " +
                std::to_string(thread_count_after));
    }
    std::cout << "connection_count_delta=" << connection_count_delta << "\n";
    std::cout << "thread_count_unchanged count=" << thread_count_after << "\n";
    std::cout << "accept_path=good_port+cross_port both_ready_on_return=yes\n";

    mlc::Task<mlc::reactor::HttpsReactorResult> early =
        start_get(port_url(good_port, "/binary"), 5000);
    mlc::Task<mlc::reactor::HttpsReactorResult> stalled =
        start_get(port_url(silent_port, "/silent"), 500);
    const mlc::reactor::HttpsReactorResult stalled_result = stalled.block_on();
    if (stalled_result.failure_code != 4) {
        return fail(5, "silent failure=" + std::to_string(stalled_result.failure_code));
    }
    if (!early.is_ready()) {
        return fail(5, "early request was not ready when the silent timeout returned");
    }
    const mlc::reactor::HttpsReactorResult early_result = early.block_on();
    if (!succeeded(early_result)) {
        return fail(
            5,
            "early status=" + std::to_string(early_result.status) +
                " failure=" + std::to_string(early_result.failure_code));
    }
    std::cout << "silent_overlap failure=" << stalled_result.failure_code
              << " status=" << early_result.status << "\n";
    return 0;
}
