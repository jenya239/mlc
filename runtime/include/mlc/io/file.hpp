#ifndef MLC_FILE_HPP
#define MLC_FILE_HPP

#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <cstdint>
#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"

namespace mlc::file {

// File handle wrapper with RAII
class File {
private:
    std::fstream stream_;
    std::string path_;
    bool is_open_;

public:
    File() : is_open_(false) {}

    explicit File(const std::string& path)
        : path_(path), is_open_(false) {}

    ~File() {
        close();
    }

    // Delete copy, allow move
    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&& other) noexcept
        : stream_(std::move(other.stream_))
        , path_(std::move(other.path_))
        , is_open_(other.is_open_) {
        other.is_open_ = false;
    }

    File& operator=(File&& other) noexcept {
        if (this != &other) {
            close();
            stream_ = std::move(other.stream_);
            path_ = std::move(other.path_);
            is_open_ = other.is_open_;
            other.is_open_ = false;
        }
        return *this;
    }

    bool open_read() {
        close();
        stream_.open(path_, std::ios::in);
        is_open_ = stream_.is_open();
        return is_open_;
    }

    bool open_write() {
        close();
        stream_.open(path_, std::ios::out | std::ios::trunc);
        is_open_ = stream_.is_open();
        return is_open_;
    }

    bool open_append() {
        close();
        stream_.open(path_, std::ios::out | std::ios::app);
        is_open_ = stream_.is_open();
        return is_open_;
    }

    void close() {
        if (is_open_ && stream_.is_open()) {
            stream_.close();
            is_open_ = false;
        }
    }

    bool is_open() const {
        return is_open_;
    }

    const std::string& path() const {
        return path_;
    }

    // Read entire file as string (works for seekable and non-seekable streams)
    std::optional<mlc::String> read_all() {
        if (!is_open_) return std::nullopt;

        std::ostringstream output_stream;
        output_stream << stream_.rdbuf();
        return mlc::String(output_stream.str());
    }

    // Read one line
    std::optional<mlc::String> read_line() {
        if (!is_open_) return std::nullopt;

        std::string line;
        if (std::getline(stream_, line)) {
            return mlc::String(line);
        }
        return std::nullopt;
    }

    // Read all lines
    std::vector<mlc::String> read_lines() {
        std::vector<mlc::String> lines;
        if (!is_open_) return lines;

        std::string line;
        while (std::getline(stream_, line)) {
            lines.push_back(mlc::String(line));
        }
        return lines;
    }

    // Write string to file
    bool write(const mlc::String& content) {
        if (!is_open_) return false;
        stream_ << content.as_std_string();
        return stream_.good();
    }

    // Write line to file (adds newline)
    bool write_line(const mlc::String& line) {
        if (!is_open_) return false;
        stream_ << line.as_std_string() << '\n';
        return stream_.good();
    }

    // Write multiple lines
    bool write_lines(const std::vector<mlc::String>& lines) {
        if (!is_open_) return false;
        for (const auto& line : lines) {
            stream_ << line.as_std_string() << '\n';
            if (!stream_.good()) return false;
        }
        return true;
    }

    // Check if at end of file
    bool eof() const {
        return stream_.eof();
    }

    // Flush the stream
    void flush() {
        if (is_open_) {
            stream_.flush();
        }
    }
};

// Convenience functions for reading files

inline mlc::String read_to_string(const mlc::String& path) {
    std::ifstream file(path.as_std_string());
    if (!file.is_open()) {
        return mlc::String("");
    }

    // streambuf read: safe for pipes /dev/stdin (seekg/tellg would yield size_t(-1))
    std::ostringstream output_stream;
    output_stream << file.rdbuf();
    return mlc::String(output_stream.str());
}

inline mlc::Array<mlc::String> read_lines(const mlc::String& path) {
    mlc::Array<mlc::String> lines;
    std::ifstream file(path.as_std_string());
    if (!file.is_open()) {
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(mlc::String(line));
    }

    return lines;
}

// Convenience functions for writing files

inline bool write_string(const mlc::String& path, const mlc::String& content) {
    std::filesystem::path p(path.as_std_string());
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    std::ofstream file(path.as_std_string(), std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    file << content.as_std_string();
    return file.good();
}

inline bool write_lines(const mlc::String& path, const std::vector<mlc::String>& lines) {
    std::ofstream file(path.as_std_string(), std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& line : lines) {
        file << line.as_std_string() << '\n';
        if (!file.good()) return false;
    }

    return true;
}

inline bool append_string(const mlc::String& path, const mlc::String& content) {
    std::ofstream file(path.as_std_string(), std::ios::out | std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << content.as_std_string();
    return file.good();
}

inline bool append_line(const mlc::String& path, const mlc::String& line) {
    std::ofstream file(path.as_std_string(), std::ios::out | std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << line.as_std_string() << '\n';
    return file.good();
}

// File system operations

inline bool exists(const mlc::String& path) {
    std::ifstream file(path.as_std_string());
    return file.good();
}

// TRACK_STDLIB_IO_FS STEP=1 — follows symlinks; false if missing (error_code).
inline bool is_directory(const mlc::String& path) {
    std::error_code error_code;
    return std::filesystem::is_directory(
        std::filesystem::path(path.as_std_string()), error_code);
}

inline bool is_regular_file(const mlc::String& path) {
    std::error_code error_code;
    return std::filesystem::is_regular_file(
        std::filesystem::path(path.as_std_string()), error_code);
}

// Absolute lexically-normalized path (missing path ok; uses cwd for relative).
inline mlc::String absolute_path(const mlc::String& path) {
    std::error_code error_code;
    std::filesystem::path resolved =
        std::filesystem::absolute(
            std::filesystem::path(path.as_std_string()), error_code)
            .lexically_normal();
    if (error_code) {
        return path;
    }
    std::string text = resolved.string();
    if (text.size() > 1 && text.back() == '/') {
        text.pop_back();
    }
    return mlc::String(text);
}

// TRACK_STDLIB_IO_FS STEP=4 — parents ok; true if created or already a directory.
inline bool create_directories(const mlc::String& path) {
    std::error_code error_code;
    const std::filesystem::path directory(path.as_std_string());
    if (std::filesystem::is_directory(directory, error_code)) {
        return true;
    }
    error_code.clear();
    (void)std::filesystem::create_directories(directory, error_code);
    if (error_code) {
        return false;
    }
    return std::filesystem::is_directory(directory, error_code);
}

// TRACK_STDLIB_IO_FS STEP=2 — non-recursive; skip . / ..; lexicographic by name.
inline void list_dir_fill(
    const mlc::String& path,
    mlc::Array<mlc::String>& names,
    mlc::Array<int32_t>& is_directory_flags
) {
    names = mlc::Array<mlc::String>{};
    is_directory_flags = mlc::Array<int32_t>{};
    std::error_code error_code;
    const std::filesystem::path directory(path.as_std_string());
    if (!std::filesystem::is_directory(directory, error_code)) {
        return;
    }
    std::vector<std::pair<std::string, int32_t>> entries;
    for (const auto& directory_entry :
         std::filesystem::directory_iterator(directory, error_code)) {
        if (error_code) {
            names = mlc::Array<mlc::String>{};
            is_directory_flags = mlc::Array<int32_t>{};
            return;
        }
        const std::string name = directory_entry.path().filename().string();
        if (name == "." || name == "..") {
            continue;
        }
        std::error_code type_error;
        const int32_t directory_flag =
            directory_entry.is_directory(type_error) ? 1 : 0;
        entries.push_back({name, directory_flag});
    }
    std::sort(
        entries.begin(),
        entries.end(),
        [](const std::pair<std::string, int32_t>& left,
           const std::pair<std::string, int32_t>& right) {
            return left.first < right.first;
        }
    );
    for (const auto& entry : entries) {
        names.push_back(mlc::String(entry.first));
        is_directory_flags.push_back(entry.second);
    }
}

inline mlc::Array<mlc::String> list_dir_names(const mlc::String& path) {
    mlc::Array<mlc::String> names;
    mlc::Array<int32_t> is_directory_flags;
    list_dir_fill(path, names, is_directory_flags);
    return names;
}

inline mlc::Array<int32_t> list_dir_is_directory_flags(const mlc::String& path) {
    mlc::Array<mlc::String> names;
    mlc::Array<int32_t> is_directory_flags;
    list_dir_fill(path, names, is_directory_flags);
    return is_directory_flags;
}

inline bool remove_file(const mlc::String& path) {
    return std::remove(path.as_std_string().c_str()) == 0;
}

inline bool rename_file(const mlc::String& old_path, const mlc::String& new_path) {
    return std::rename(old_path.as_std_string().c_str(),
                      new_path.as_std_string().c_str()) == 0;
}

inline mlc::String temp_directory_base() {
    const char* temporary_directory = std::getenv("TMPDIR");
    if (temporary_directory != nullptr && temporary_directory[0] != '\0') {
        return mlc::String(temporary_directory);
    }
    return mlc::String("/tmp");
}

inline mlc::String make_temp_directory(const mlc::String& prefix) {
    std::string base = temp_directory_base().as_std_string();
    std::string pattern = base + "/" + prefix.as_std_string() + "XXXXXX";
    std::vector<char> buffer(pattern.begin(), pattern.end());
    buffer.push_back('\0');
    char* created = mkdtemp(buffer.data());
    if (created == nullptr) {
        return mlc::String("");
    }
    return mlc::String(created);
}

} // namespace mlc::file

#endif // MLC_FILE_HPP
