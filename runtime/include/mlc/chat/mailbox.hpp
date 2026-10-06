#pragma once

// Invariants:
// - At most one request is in flight until mailbox_take_ready consumes it.
// - A launch while a request is in flight returns 0 and starts nothing.
// - The worker is a detached thread: Task has no non-blocking read, and
//   dropping a spawn Task waits for that thread.
// - The worker calls block_on on its own reactor. The caller does not.
// - Nothing here cancels the transfer.

#include "mlc/reactor/https_async_bridge.hpp"

#include <exception>
#include <mutex>
#include <thread>
#include <utility>

namespace mlc::chat {

struct MailboxState {
    std::mutex mutex;
    bool request_in_flight = false;
    bool delivery_occupied = false;
    int generation = 0;
    int transport_failure = 0;
    int status = 0;
    mlc::String text;
};

struct MailboxStaged {
    int generation = 0;
    int transport_failure = 0;
    int status = 0;
    mlc::String text;
};

inline MailboxState& mailbox_state() {
    static MailboxState state;
    return state;
}

inline MailboxStaged& mailbox_staged() {
    static MailboxStaged staged;
    return staged;
}

inline void post_delivery(int generation, int transport_failure, int status, mlc::String text) {
    std::lock_guard<std::mutex> guard(mailbox_state().mutex);
    MailboxState& state = mailbox_state();
    if (state.delivery_occupied) {
        return;
    }
    state.delivery_occupied = true;
    state.generation = generation;
    state.transport_failure = transport_failure;
    state.status = status;
    state.text = std::move(text);
}

inline void run_request(int generation, https_request::HttpsRequest request) {
    try {
        mlc::Task<https_request::HttpsResult> task =
            mlc::reactor::https_send_async(std::move(request));
        https_request::HttpsResult result = mlc::block_on(std::move(task));
        if (const https_request::HttpsOk* success = std::get_if<https_request::HttpsOk>(&result)) {
            post_delivery(generation, 0, success->field0.status, success->field0.body);
            return;
        }
        if (const https_request::HttpsErr* failure = std::get_if<https_request::HttpsErr>(&result)) {
            mlc::String message = failure->field0.message;
            if (message.raw_size() == 0) {
                message = mlc::String("transport failed");
            }
            post_delivery(generation, 1, 0, std::move(message));
            return;
        }
        post_delivery(generation, 1, 0, mlc::String("transport failed"));
    } catch (const std::exception& error) {
        post_delivery(generation, 1, 0, mlc::String(error.what()));
    } catch (...) {
        post_delivery(generation, 1, 0, mlc::String("transport failed"));
    }
}

inline int transport_launch(int generation, https_request::HttpsRequest request) {
    {
        std::lock_guard<std::mutex> guard(mailbox_state().mutex);
        if (mailbox_state().request_in_flight) {
            return 0;
        }
        mailbox_state().request_in_flight = true;
    }
    request.timeout_milliseconds = 60000;
    try {
        std::thread worker([generation, request = std::move(request)]() mutable {
            run_request(generation, std::move(request));
        });
        worker.detach();
    } catch (const std::exception& error) {
        post_delivery(generation, 1, 0, mlc::String(error.what()));
    } catch (...) {
        post_delivery(generation, 1, 0, mlc::String("transport failed"));
    }
    return 1;
}

inline int mailbox_in_flight() {
    std::lock_guard<std::mutex> guard(mailbox_state().mutex);
    if (mailbox_state().request_in_flight) {
        return 1;
    }
    return 0;
}

inline int mailbox_take_ready() {
    std::lock_guard<std::mutex> guard(mailbox_state().mutex);
    MailboxState& state = mailbox_state();
    if (!state.delivery_occupied) {
        return 0;
    }
    MailboxStaged& staged = mailbox_staged();
    staged.generation = state.generation;
    staged.transport_failure = state.transport_failure;
    staged.status = state.status;
    staged.text = state.text;
    state.delivery_occupied = false;
    state.request_in_flight = false;
    state.text = mlc::String();
    return 1;
}

inline int mailbox_staged_generation() {
    return mailbox_staged().generation;
}

inline int mailbox_staged_transport_failure() {
    return mailbox_staged().transport_failure;
}

inline int mailbox_staged_status() {
    return mailbox_staged().status;
}

inline mlc::String mailbox_staged_text() {
    return mailbox_staged().text;
}

}  // namespace mlc::chat
