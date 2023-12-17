#pragma once

#include <optional>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <utility>

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

enum struct FileEncoding { UTF8, UTF16 };

using line_uid_t = int32_t;

constexpr int g_line_no_uid = {0};

enum struct LogLevel {Trace, Debug, Info, Warn, Error, Critical};

enum struct LogRecordType { Unknown, HeaderAndMessage, MessagePart };

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

template <typename T>
concept LineEntity = requires(T entity, std::string_view value_sv) {
    {entity.to_regex()} -> std::same_as<std::string_view>;
    {std::as_const(entity).clone()} -> std::same_as<T>;
    {entity.set_value(value_sv)};
    {std::as_const(entity).value()} -> std::same_as<std::string const&>;
};


} // namespace l4h::defs
