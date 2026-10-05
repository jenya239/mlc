#pragma once

#include "mlc/core/task.hpp"
#include "mlc/net/curl_abi.hpp"
#include "mlc/reactor/event_loop.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace mlc::reactor {

struct HttpsReactorResult {
    std::int32_t status = 0;
    std::int32_t failure_code = 0;
    std::string message;
    std::string body;
};

struct HttpsTransfer {
    mlc::String method;
    mlc::String url;
    mlc::String header_lines;
    mlc::String body;
    mlc::String certificate_bundle_path;
    mlc::String proxy_url;
    curl_abi::TransferState transfer;
    char error_buffer[CURL_ERROR_SIZE];
    CURL* easy = nullptr;
    curl_slist* header_list = nullptr;
    std::coroutine_handle<> continuation;
    bool complete = false;
    HttpsReactorResult result;

    HttpsTransfer() { error_buffer[0] = '\0'; }

    ~HttpsTransfer() {
        if (header_list != nullptr) {
            curl_slist_free_all(header_list);
            header_list = nullptr;
        }
        if (easy != nullptr) {
            curl_easy_cleanup(easy);
            easy = nullptr;
        }
    }

    HttpsTransfer(const HttpsTransfer&) = delete;
    HttpsTransfer& operator=(const HttpsTransfer&) = delete;
};

struct HttpsTransferAwaiter {
    std::shared_ptr<HttpsTransfer> transfer;

    [[nodiscard]] bool await_ready() const noexcept { return transfer->complete; }

    void await_suspend(std::coroutine_handle<> continuation) const {
        transfer->continuation = continuation;
    }

    [[nodiscard]] HttpsReactorResult await_resume() const { return transfer->result; }
};

inline mlc::Task<HttpsReactorResult> wait_for_https_transfer_body(std::shared_ptr<HttpsTransfer> transfer) {
    HttpsReactorResult result = co_await HttpsTransferAwaiter{std::move(transfer)};
    co_return result;
}

inline mlc::Task<HttpsReactorResult> wait_for_https_transfer(std::shared_ptr<HttpsTransfer> transfer) {
    mlc::Task<HttpsReactorResult> task = wait_for_https_transfer_body(std::move(transfer));
    task.mark_as_reactor();
    return task;
}

inline mlc::Task<HttpsReactorResult> https_transfer_failure_body(
    std::int32_t failure_code, std::string message) {
    HttpsReactorResult result;
    result.failure_code = failure_code;
    result.message = std::move(message);
    co_return result;
}

inline mlc::Task<HttpsReactorResult> https_transfer_failure(
    std::int32_t failure_code, std::string message) {
    mlc::Task<HttpsReactorResult> task =
        https_transfer_failure_body(failure_code, std::move(message));
    task.mark_as_reactor();
    return task;
}

inline void complete_https_transfer(const std::shared_ptr<HttpsTransfer>& transfer, CURLcode code) {
    long response_code = 0;
    if (transfer->easy != nullptr) {
        curl_easy_getinfo(transfer->easy, CURLINFO_RESPONSE_CODE, &response_code);
    }
    transfer->result.status = static_cast<std::int32_t>(response_code);
    transfer->result.failure_code =
        curl_abi::classify_curl_transfer(code, transfer->transfer.body_too_large);
    if (transfer->result.failure_code == 0 && transfer->transfer.header_block_too_large) {
        transfer->result.failure_code = 7;
    }
    const mlc::String message = curl_abi::curl_transfer_message(code, transfer->error_buffer);
    if (message.raw_size() > 0 && message.raw_data() != nullptr) {
        transfer->result.message.assign(message.raw_data(), message.raw_size());
    }
    transfer->result.body = transfer->transfer.body;
    transfer->complete = true;
}

inline mlc::Task<HttpsReactorResult> start_https_transfer(
    mlc::String method,
    mlc::String url,
    mlc::String header_lines,
    mlc::String body,
    std::int32_t timeout_milliseconds,
    std::int32_t max_response_bytes,
    mlc::String certificate_bundle_path,
    mlc::String proxy_url,
    mlc::concurrency::StopToken stop_token) {
    if (stop_token.requested()) {
        mlc::Task<HttpsReactorResult> task = https_transfer_failure(7, "stop requested");
        task.resume();
        return task;
    }

    curl_abi::prepare_curl();
    auto transfer = std::make_shared<HttpsTransfer>();
    transfer->method = std::move(method);
    transfer->url = std::move(url);
    transfer->header_lines = std::move(header_lines);
    transfer->body = std::move(body);
    transfer->certificate_bundle_path = std::move(certificate_bundle_path);
    transfer->proxy_url = std::move(proxy_url);
    transfer->transfer.maximum_body_bytes =
        max_response_bytes > 0 ? static_cast<std::size_t>(max_response_bytes) : 0;

    EventLoop& loop = EventLoop::current();
    CURLM* multi = loop.ensure_curl_multi();
    transfer->easy = curl_easy_init();
    if (transfer->easy == nullptr || multi == nullptr) {
        mlc::Task<HttpsReactorResult> task = https_transfer_failure(7, "curl easy init failed");
        task.resume();
        return task;
    }
    if (!curl_abi::configure_https_easy_handle(
            transfer->easy,
            &transfer->transfer,
            transfer->error_buffer,
            transfer->method,
            transfer->url,
            transfer->header_lines,
            transfer->body,
            timeout_milliseconds,
            transfer->certificate_bundle_path,
            transfer->proxy_url,
            false,
            curl_abi::https_body_write,
            &transfer->transfer,
            &transfer->header_list)) {
        mlc::Task<HttpsReactorResult> task = https_transfer_failure(7, "curl header list failed");
        task.resume();
        return task;
    }

    const CURLMcode add_code = curl_multi_add_handle(multi, transfer->easy);
    if (add_code != CURLM_OK) {
        mlc::Task<HttpsReactorResult> task = https_transfer_failure(7, "curl multi add failed");
        task.resume();
        return task;
    }

    std::shared_ptr<HttpsTransfer> registered = transfer;
    loop.register_curl_completion(transfer->easy, [registered](CURLcode code) {
        complete_https_transfer(registered, code);
        return registered->continuation;
    });
    loop.request_curl_service();

    mlc::Task<HttpsReactorResult> task = wait_for_https_transfer(std::move(transfer));
    task.resume();
    return task;
}

}  // namespace mlc::reactor
