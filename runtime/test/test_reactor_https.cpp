#include "mlc/net/curl_abi.hpp"
#include "mlc/reactor/https_transfer.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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

std::string good_url(const std::string& path) {
    return "https://127.0.0.1:" + read_text(directory_path() + "/good_port") + path;
}

std::string wrong_url(const std::string& path) {
    return "https://127.0.0.1:" + read_text(directory_path() + "/wrong_port") + path;
}

std::string authority_path() { return read_text(directory_path() + "/certificate_authority_path"); }

int read_counter(const std::string& name) {
    return std::stoi(read_text(directory_path() + "/" + name));
}

struct ObservedTransfer {
    std::int32_t status = 0;
    std::int32_t failure_code = 0;
    std::string message;
    std::string body;
};

std::string text_of(const mlc::String& text) {
    if (text.raw_size() == 0 || text.raw_data() == nullptr) return std::string();
    return std::string(text.raw_data(), text.raw_size());
}

ObservedTransfer observe_blocking(const std::string& url) {
    const std::int32_t handle = mlc::curl_abi::perform_request(
        mlc::String("GET"),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(""),
        5000,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""));
    ObservedTransfer observed;
    observed.status = mlc::curl_abi::result_status(handle);
    observed.failure_code = mlc::curl_abi::result_failure_code(handle);
    observed.message = text_of(mlc::curl_abi::result_message(handle));
    observed.body = text_of(mlc::curl_abi::result_body(handle));
    mlc::curl_abi::release_result(handle);
    return observed;
}

ObservedTransfer observe_reactor(const std::string& url, mlc::concurrency::StopToken stop_token) {
    mlc::Task<mlc::reactor::HttpsReactorResult> task = mlc::reactor::start_https_transfer(
        mlc::String("GET"),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(""),
        5000,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""),
        std::move(stop_token));
    if (!task.is_ready()) {
        mlc::reactor::EventLoop::current().run_until([&] { return task.is_ready(); });
    }
    const mlc::reactor::HttpsReactorResult result = task.block_on();
    ObservedTransfer observed;
    observed.status = result.status;
    observed.failure_code = result.failure_code;
    observed.message = result.message;
    observed.body = result.body;
    return observed;
}

int fail(int code, const std::string& message) {
    std::cerr << message << "\n";
    return code;
}

bool same_transfer(const ObservedTransfer& blocking, const ObservedTransfer& reactor) {
    return blocking.status == reactor.status && blocking.failure_code == reactor.failure_code &&
           blocking.message == reactor.message && blocking.body == reactor.body;
}

std::string describe(const ObservedTransfer& observed) {
    return "status=" + std::to_string(observed.status) +
           " failure=" + std::to_string(observed.failure_code) + " message=" + observed.message +
           " body_bytes=" + std::to_string(observed.body.size());
}

ObservedTransfer observe_blocking_exchange(
    const std::string& method, const std::string& url, const std::string& body) {
    const std::int32_t handle = mlc::curl_abi::perform_request(
        mlc::String(method.c_str()),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(body.c_str()),
        5000,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""));
    ObservedTransfer observed;
    observed.status = mlc::curl_abi::result_status(handle);
    observed.failure_code = mlc::curl_abi::result_failure_code(handle);
    observed.message = text_of(mlc::curl_abi::result_message(handle));
    observed.body = text_of(mlc::curl_abi::result_body(handle));
    mlc::curl_abi::release_result(handle);
    return observed;
}

ObservedTransfer observe_reactor_exchange(
    const std::string& method,
    const std::string& url,
    const std::string& body,
    mlc::concurrency::StopToken stop_token) {
    mlc::Task<mlc::reactor::HttpsReactorResult> task = mlc::reactor::start_https_transfer(
        mlc::String(method.c_str()),
        mlc::String(url.c_str()),
        mlc::String(""),
        mlc::String(body.c_str()),
        5000,
        4194304,
        mlc::String(authority_path().c_str()),
        mlc::String(""),
        std::move(stop_token));
    if (!task.is_ready()) {
        mlc::reactor::EventLoop::current().run_until([&] { return task.is_ready(); });
    }
    const mlc::reactor::HttpsReactorResult result = task.block_on();
    ObservedTransfer observed;
    observed.status = result.status;
    observed.failure_code = result.failure_code;
    observed.message = result.message;
    observed.body = result.body;
    return observed;
}

int compare_post_echo() {
    const std::string url = good_url("/echo");
    const std::string body = "echo-body";
    mlc::concurrency::StopSource stop_source;
    const ObservedTransfer blocking = observe_blocking_exchange("POST", url, body);
    const ObservedTransfer reactor = observe_reactor_exchange("POST", url, body, stop_source.token());
    if (!same_transfer(blocking, reactor) || reactor.status != 200 || reactor.body != body) {
        return fail(
            2,
            "post_echo blocking " + describe(blocking) + " reactor " + describe(reactor));
    }
    std::cout << "post_echo status=200 body=echo-body\n";
    return 0;
}

int compare_case(const std::string& url, int code, const std::string& name) {
    mlc::concurrency::StopSource stop_source;
    const ObservedTransfer blocking = observe_blocking(url);
    const ObservedTransfer reactor = observe_reactor(url, stop_source.token());
    if (!same_transfer(blocking, reactor)) {
        return fail(code, name + " blocking " + describe(blocking) + " reactor " + describe(reactor));
    }
    std::cout << name << " " << describe(reactor) << "\n";
    return 0;
}

}  // namespace

int main() {
    if (directory_path().empty()) {
        return fail(1, "REACTOR_HTTPS_DIR is empty");
    }

    const int body_code = compare_case(good_url("/binary"), 2, "get_body");
    if (body_code != 0) return body_code;
    const int post_code = compare_post_echo();
    if (post_code != 0) return post_code;
    const int certificate_code = compare_case(wrong_url("/echo"), 3, "wrong_certificate");
    if (certificate_code != 0) return certificate_code;
    const int connect_code = compare_case("https://127.0.0.1:1/closed", 4, "closed_port");
    if (connect_code != 0) return connect_code;
    const int resolve_code =
        compare_case("https://mlc-reactor-unresolvable.invalid/resource", 5, "unresolvable_name");
    if (resolve_code != 0) return resolve_code;

    const int connections_before = read_counter("connection_count");
    mlc::concurrency::StopSource stop_source;
    stop_source.request();
    const ObservedTransfer stopped = observe_reactor(good_url("/binary"), stop_source.token());
    if (stopped.failure_code != 7 || stopped.message != "stop requested") {
        return fail(6, "stop requested result " + describe(stopped));
    }
    if (read_counter("connection_count") != connections_before) {
        return fail(6, "stop requested opened a connection");
    }
    std::cout << "stop_requested_before_connect\n";
    return 0;
}
