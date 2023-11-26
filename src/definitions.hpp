#pragma once

#include <optional>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace l4h {
    class LineParser;
}

namespace l4h::defs {

constexpr int g_size_1kb = 1'024;

consteval size_t operator ""_kb (unsigned long long value) {
    return value * g_size_1kb;
}

consteval size_t operator ""_mb(unsigned long long value) {
    return value * g_size_1kb * g_size_1kb;
}

enum class FileEncoding { UTF8, UTF16 };

using line_uid_t = int32_t;

constexpr int g_line_no_uid = {0};

struct StreamLineData {
    long long start_pos{g_line_no_uid};
    line_uid_t id{g_line_no_uid};
    std::string line;
};

struct StreamConfig {
    std::filesystem::path file_path;
    int chunk_size{32_kb};
    FileEncoding encoding{FileEncoding::UTF8};
    std::shared_ptr<LineParser> line_parser;
};

const char g_new_line = '\n';

} // namespace l4h::defs
