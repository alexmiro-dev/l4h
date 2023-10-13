#pragma once

#include <optional>
#include <vector>
#include <cstdint>
#include <filesystem>

#include "Header.hpp"

namespace omlog::defs {

    constexpr int Size1Kb = 1'024;

    consteval size_t operator ""_kb (unsigned long long value) {
        return value * Size1Kb;
    }

    consteval size_t operator ""_mb(unsigned long long value) {
        return value * Size1Kb * Size1Kb;
    }

    enum class FileEncoding { UTF8, UTF16 };

    struct StreamLineData {
        long long start_pos{0};
        int32_t id{0};
        std::string line;
    };

    struct StreamConfig {
        std::filesystem::path file_path;
        int chunk_size{32_kb};
        FileEncoding encoding{FileEncoding::UTF8};
        header::HeaderPattern header_pattern;
    };

    const char g_new_line = '\n';

} // namespace omlog::defs
