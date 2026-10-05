#pragma once

#include "mlc/reactor/https_transfer.hpp"

#include "https_request.hpp"
#include "https_async.hpp"

#include <utility>

namespace mlc::reactor {

inline https_request::HttpsResult https_result_from_reactor(const HttpsReactorResult& reactor_result) {
    if (reactor_result.failure_code != 0) {
        return https_request::HttpsErr{https_request::HttpsFailure{
            https_request::https_failure_kind_from_code(reactor_result.failure_code),
            mlc::String(reactor_result.message)}};
    }
    return https_request::HttpsOk{https_request::HttpsResponse{
        reactor_result.status,
        https_request::parse_response_header_block(mlc::String(reactor_result.header_block)),
        mlc::String(reactor_result.body)}};
}

inline mlc::Task<https_request::HttpsResult> task_ready_https_result_body(
    https_request::HttpsResult value) {
    co_return std::move(value);
}

inline mlc::Task<https_request::HttpsResult> task_ready_https_result(https_request::HttpsResult value) {
    mlc::Task<https_request::HttpsResult> task = task_ready_https_result_body(std::move(value));
    task.mark_as_reactor();
    task.resume();
    return task;
}

inline mlc::Task<https_request::HttpsResult> https_result_from_transfer(
    mlc::Task<HttpsReactorResult> transfer) {
    HttpsReactorResult reactor_result = transfer.block_on();
    co_return https_result_from_reactor(reactor_result);
}

inline mlc::Task<https_request::HttpsResult> https_send_async(https_request::HttpsRequest request) {
    const mlc::String problem = https_request::https_request_problem(request);
    if (problem.raw_size() != 0) {
        return task_ready_https_result(https_request::HttpsErr{https_request::HttpsFailure{
            https_request::InvalidRequest{},
            problem}});
    }
    mlc::Task<HttpsReactorResult> transfer = start_https_transfer(
        request.method,
        request.url,
        https_request::https_header_lines(request.headers),
        request.body,
        request.timeout_milliseconds,
        request.max_response_bytes,
        request.ca_bundle_path,
        request.proxy_url,
        mlc::concurrency::StopSource{}.token());
    mlc::Task<https_request::HttpsResult> task = https_result_from_transfer(std::move(transfer));
    task.mark_as_reactor();
    return task;
}

inline mlc::Task<https_async::HttpsResultPair> task_ready_https_result_pair_body(
    https_async::HttpsResultPair value) {
    co_return std::move(value);
}

inline mlc::Task<https_async::HttpsResultPair> task_ready_https_result_pair(
    https_async::HttpsResultPair value) {
    mlc::Task<https_async::HttpsResultPair> task = task_ready_https_result_pair_body(std::move(value));
    task.mark_as_reactor();
    task.resume();
    return task;
}

}  // namespace mlc::reactor
