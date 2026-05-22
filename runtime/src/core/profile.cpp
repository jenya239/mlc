#include "mlc/core/profile.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace mlc::profile {
namespace {

struct PhaseEntry {
    int64_t total_nanos = 0;
    int64_t call_count = 0;
    int64_t peak_rss_kib = 0;
};

int64_t g_peak_rss_kib = 0;
std::unordered_map<std::string, PhaseEntry> g_phases;
std::vector<std::pair<std::string, int64_t>> g_scope_stack;

int64_t read_rss_kib_from_proc() {
    std::ifstream status("/proc/self/status");
    if (!status.is_open()) {
        return 0;
    }
    std::string line;
    while (std::getline(status, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            long value = 0;
            char unit[8] = {};
            if (std::sscanf(line.c_str(), "VmRSS: %ld %7s", &value, unit) >= 1) {
                return static_cast<int64_t>(value);
            }
            return 0;
        }
    }
    return 0;
}

void note_rss_peak() {
    const int64_t current = read_rss_kib_from_proc();
    if (current > g_peak_rss_kib) {
        g_peak_rss_kib = current;
    }
}

} // namespace

int64_t monotonic_nanos() noexcept {
    using clock = std::chrono::steady_clock;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               clock::now().time_since_epoch())
        .count();
}

int64_t current_rss_kib() noexcept {
    return read_rss_kib_from_proc();
}

int64_t peak_rss_kib() noexcept {
    note_rss_peak();
    return g_peak_rss_kib;
}

void reset() noexcept {
    g_phases.clear();
    g_scope_stack.clear();
    g_peak_rss_kib = 0;
}

void scope_begin(const mlc::String& label) noexcept {
    note_rss_peak();
    g_scope_stack.emplace_back(label.as_std_string(), monotonic_nanos());
}

void scope_end(const mlc::String& label) noexcept {
    if (g_scope_stack.empty()) {
        return;
    }
    const auto started = g_scope_stack.back();
    g_scope_stack.pop_back();
    const int64_t elapsed = monotonic_nanos() - started.second;
    PhaseEntry& entry = g_phases[started.first];
    entry.total_nanos += elapsed;
    entry.call_count += 1;
    note_rss_peak();
    entry.peak_rss_kib = std::max(entry.peak_rss_kib, g_peak_rss_kib);
    (void)label;
}

void print_report() noexcept {
    note_rss_peak();
    std::fprintf(stderr, "[mlcc profile] peak RSS: %lld KiB\n",
                 static_cast<long long>(g_peak_rss_kib));

    std::vector<std::pair<std::string, PhaseEntry>> rows;
    rows.reserve(g_phases.size());
    for (const auto& item : g_phases) {
        rows.push_back(item);
    }
    std::sort(rows.begin(), rows.end(), [](const auto& left, const auto& right) {
        return left.second.total_nanos > right.second.total_nanos;
    });

    int64_t total_nanos = 0;
    for (const auto& row : rows) {
        total_nanos += row.second.total_nanos;
    }

    std::fprintf(stderr, "[mlcc profile] phases (sorted by time):\n");
    std::fprintf(stderr, "%-32s %10s %8s %10s %8s\n",
                 "phase", "ms", "calls", "ms/call", "rss_kib");
    for (const auto& row : rows) {
        const double milliseconds =
            static_cast<double>(row.second.total_nanos) / 1000000.0;
        const double milliseconds_per_call =
            row.second.call_count > 0
                ? milliseconds / static_cast<double>(row.second.call_count)
                : 0.0;
        std::fprintf(stderr, "%-32s %10.2f %8lld %10.2f %8lld\n",
                     row.first.c_str(),
                     milliseconds,
                     static_cast<long long>(row.second.call_count),
                     milliseconds_per_call,
                     static_cast<long long>(row.second.peak_rss_kib));
    }
    if (total_nanos > 0) {
        std::fprintf(stderr, "[mlcc profile] accounted: %.2f ms\n",
                     static_cast<double>(total_nanos) / 1000000.0);
    }
}

} // namespace mlc::profile
