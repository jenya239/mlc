#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>
#include <variant>
#include <future>
#include <thread>
#include "../core/task.hpp"

namespace mlc::net {

// HTTP method enum matching MLC HttpMethod
enum class HttpMethod {
    Get,
    Post,
    Put,
    Delete,
    Patch,
    Head,
    Options
};

// Headers type - vector of key-value pairs
struct Headers {
    std::vector<std::pair<std::string, std::string>> entries;

    void set(const std::string& key, const std::string& value) {
        entries.emplace_back(key, value);
    }

    std::optional<std::string> get(const std::string& key) const {
        for (const auto& [k, v] : entries) {
            if (k == key) return v;
        }
        return std::nullopt;
    }
};

// Request options
struct RequestInit {
    HttpMethod method = HttpMethod::Get;
    Headers headers;
    std::optional<std::string> body;
};

// HTTP Response
struct Response {
    int status;
    std::string status_text;
    Headers headers;
    std::string url;
    std::string body;

    bool ok() const { return status >= 200 && status < 300; }
};

// Result type for fetch operations
template<typename T, typename E>
using Result = std::variant<T, E>;

namespace detail {

// Callback for writing response body
inline size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* response_body = static_cast<std::string*>(userdata);
    response_body->append(ptr, size * nmemb);
    return size * nmemb;
}

// Callback for parsing response headers
inline size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
    auto* headers = static_cast<Headers*>(userdata);
    std::string line(buffer, size * nitems);

    // Skip status line and empty lines
    if (line.find("HTTP/") == 0 || line == "\r\n") {
        return size * nitems;
    }

    // Parse "Key: Value" format
    auto colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // Trim whitespace
        while (!value.empty() && (value.front() == ' ' || value.front() == '\t')) {
            value.erase(0, 1);
        }
        while (!value.empty() && (value.back() == '\r' || value.back() == '\n')) {
            value.pop_back();
        }

        headers->set(key, value);
    }

    return size * nitems;
}

// Get HTTP method string
inline const char* method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::Get: return "GET";
        case HttpMethod::Post: return "POST";
        case HttpMethod::Put: return "PUT";
        case HttpMethod::Delete: return "DELETE";
        case HttpMethod::Patch: return "PATCH";
        case HttpMethod::Head: return "HEAD";
        case HttpMethod::Options: return "OPTIONS";
        default: return "GET";
    }
}

// Synchronous fetch implementation using libcurl
inline Result<Response, std::string> fetch_sync(const std::string& url, const RequestInit& options) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return std::string("Failed to initialize CURL");
    }

    Response response;
    response.url = url;

    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Set method
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method_to_string(options.method));

    // Set headers
    struct curl_slist* header_list = nullptr;
    for (const auto& [key, value] : options.headers.entries) {
        std::string header = key + ": " + value;
        header_list = curl_slist_append(header_list, header.c_str());
    }
    if (header_list) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    // Set body for POST/PUT/PATCH
    if (options.body.has_value()) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body->c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, options.body->size());
    }

    // Set callbacks for response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);

    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // SSL verification (can be disabled for testing)
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // Perform request
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::string error = curl_easy_strerror(res);
        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);
        return error;
    }

    // Get status code
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    response.status = static_cast<int>(http_code);

    // Set status text based on code
    switch (response.status) {
        case 200: response.status_text = "OK"; break;
        case 201: response.status_text = "Created"; break;
        case 204: response.status_text = "No Content"; break;
        case 301: response.status_text = "Moved Permanently"; break;
        case 302: response.status_text = "Found"; break;
        case 304: response.status_text = "Not Modified"; break;
        case 400: response.status_text = "Bad Request"; break;
        case 401: response.status_text = "Unauthorized"; break;
        case 403: response.status_text = "Forbidden"; break;
        case 404: response.status_text = "Not Found"; break;
        case 500: response.status_text = "Internal Server Error"; break;
        case 502: response.status_text = "Bad Gateway"; break;
        case 503: response.status_text = "Service Unavailable"; break;
        default: response.status_text = "Unknown"; break;
    }

    // Cleanup
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    return response;
}

} // namespace detail

// Async fetch - returns Task<Result<Response, string>>
// Note: Takes url by value to ensure it survives coroutine suspension
inline mlc::Task<Result<Response, std::string>> fetch(std::string url) {
    // Run synchronous fetch in a separate thread and wrap in Task
    auto result = std::async(std::launch::async, [url = std::move(url)]() {
        return detail::fetch_sync(url, RequestInit{});
    });

    // For now, we block and return - proper async integration would use
    // an event loop or thread pool integrated with the coroutine scheduler
    co_return result.get();
}

// Async fetch with options
// Note: Takes parameters by value to ensure they survive coroutine suspension
inline mlc::Task<Result<Response, std::string>> fetch_with_options(
    std::string url,
    RequestInit options
) {
    auto result = std::async(std::launch::async, [url = std::move(url), options = std::move(options)]() {
        return detail::fetch_sync(url, options);
    });

    co_return result.get();
}

// Synchronous versions for convenience
inline Result<Response, std::string> fetch_sync(const std::string& url) {
    return detail::fetch_sync(url, RequestInit{});
}

inline Result<Response, std::string> fetch_sync(const std::string& url, const RequestInit& options) {
    return detail::fetch_sync(url, options);
}

// CURL global initialization helper
struct CurlInit {
    CurlInit() { curl_global_init(CURL_GLOBAL_ALL); }
    ~CurlInit() { curl_global_cleanup(); }
};

// Static instance to ensure CURL is initialized
inline CurlInit& curl_init() {
    static CurlInit instance;
    return instance;
}

} // namespace mlc::net
