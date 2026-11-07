#pragma once

// Invariants:
// - Separate from the chat mailbox: one companion request does not block chat.
// - The worker uses curl_easy_perform. The gallery frame does not pump this transfer.
// - At most one request is in flight until companion_take_ready or companion_abandon.
// - companion_abandon invalidates that attempt. A late completion is ignored.
// - A launch while a request is in flight returns 0 and starts nothing.

#include "mlc/net/curl_abi.hpp"

#include "https_request.hpp"

#include <exception>
#include <mutex>
#include <thread>
#include <utility>

namespace mlc::companion {

struct CompanionMailboxState {
    std::mutex mutex;
    bool request_in_flight = false;
    bool delivery_occupied = false;
    int generation = 0;
    int transport_failure = 0;
    int status = 0;
    mlc::String text;
};

struct CompanionMailboxStaged {
    int transport_failure = 0;
    int status = 0;
    mlc::String text;
};

inline CompanionMailboxState& companion_state() {
    static CompanionMailboxState state;
    return state;
}

inline CompanionMailboxStaged& companion_staged() {
    static CompanionMailboxStaged staged;
    return staged;
}

inline void companion_post(int generation, int transport_failure, int status, mlc::String text) {
    std::lock_guard<std::mutex> guard(companion_state().mutex);
    CompanionMailboxState& state = companion_state();
    if (generation != state.generation || state.delivery_occupied) {
        return;
    }
    state.delivery_occupied = true;
    state.transport_failure = transport_failure;
    state.status = status;
    state.text = std::move(text);
}

inline void companion_run(int generation, https_request::HttpsRequest request) {
    CURL* easy = nullptr;
    curl_slist* header_list = nullptr;
    try {
        mlc::curl_abi::prepare_curl();
        easy = curl_easy_init();
        if (easy == nullptr) {
            companion_post(generation, 1, 0, mlc::String("curl easy init failed"));
            return;
        }
        mlc::curl_abi::TransferState transfer;
        transfer.maximum_body_bytes =
            request.max_response_bytes > 0 ? static_cast<std::size_t>(request.max_response_bytes) : 65536;
        char error_buffer[CURL_ERROR_SIZE];
        error_buffer[0] = '\0';
        const mlc::String header_lines = https_request::https_header_lines(request.headers);
        const bool configured = mlc::curl_abi::configure_https_easy_handle(
            easy,
            &transfer,
            error_buffer,
            request.method,
            request.url,
            header_lines,
            request.body,
            20000,
            request.ca_bundle_path,
            request.proxy_url,
            false,
            mlc::curl_abi::https_body_write,
            &transfer,
            &header_list);
        if (!configured) {
            curl_easy_cleanup(easy);
            companion_post(generation, 1, 0, mlc::String("curl header list failed"));
            return;
        }
        curl_easy_setopt(easy, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
        const CURLcode code = curl_easy_perform(easy);
        long status = 0;
        curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &status);
        if (header_list != nullptr) {
            curl_slist_free_all(header_list);
            header_list = nullptr;
        }
        curl_easy_cleanup(easy);
        easy = nullptr;
        const int failure = mlc::curl_abi::classify_curl_transfer(code, transfer.body_too_large);
        if (failure != 0) {
            mlc::String message = mlc::curl_abi::curl_transfer_message(code, error_buffer);
            if (message.raw_size() == 0) {
                message = mlc::String("transport failed");
            }
            companion_post(generation, 1, 0, std::move(message));
            return;
        }
        companion_post(generation, 0, static_cast<int>(status), mlc::String(transfer.body));
    } catch (const std::exception& error) {
        if (header_list != nullptr) curl_slist_free_all(header_list);
        if (easy != nullptr) curl_easy_cleanup(easy);
        companion_post(generation, 1, 0, mlc::String(error.what()));
    } catch (...) {
        if (header_list != nullptr) curl_slist_free_all(header_list);
        if (easy != nullptr) curl_easy_cleanup(easy);
        companion_post(generation, 1, 0, mlc::String("transport failed"));
    }
}

inline int transport_launch(https_request::HttpsRequest request) {
    int generation = 0;
    {
        std::lock_guard<std::mutex> guard(companion_state().mutex);
        if (companion_state().request_in_flight) {
            return 0;
        }
        companion_state().request_in_flight = true;
        companion_state().generation += 1;
        generation = companion_state().generation;
    }
    request.timeout_milliseconds = 20000;
    request.max_response_bytes = 65536;
    try {
        std::thread worker([generation, request = std::move(request)]() mutable {
            companion_run(generation, std::move(request));
        });
        worker.detach();
    } catch (const std::exception& error) {
        companion_post(generation, 1, 0, mlc::String(error.what()));
    } catch (...) {
        companion_post(generation, 1, 0, mlc::String("transport failed"));
    }
    return 1;
}

inline int companion_abandon() {
    std::lock_guard<std::mutex> guard(companion_state().mutex);
    CompanionMailboxState& state = companion_state();
    state.generation += 1;
    state.request_in_flight = false;
    state.delivery_occupied = false;
    state.text = mlc::String();
    return 1;
}

inline int companion_take_ready() {
    std::lock_guard<std::mutex> guard(companion_state().mutex);
    CompanionMailboxState& state = companion_state();
    if (!state.delivery_occupied) {
        return 0;
    }
    CompanionMailboxStaged& staged = companion_staged();
    staged.transport_failure = state.transport_failure;
    staged.status = state.status;
    staged.text = state.text;
    state.delivery_occupied = false;
    state.request_in_flight = false;
    state.text = mlc::String();
    return 1;
}

inline int companion_staged_transport_failure() {
    return companion_staged().transport_failure;
}

inline int companion_staged_status() {
    return companion_staged().status;
}

inline mlc::String companion_staged_text() {
    return companion_staged().text;
}

}  // namespace mlc::companion
