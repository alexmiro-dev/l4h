#pragma once

#include <optional>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace omlog::defs {

    constexpr int Size1Kb = 1'024;

    inline constexpr int operator ""_kb (unsigned long long value) {
        return value * Size1Kb;
    }

    enum class FileEncoding { UTF8, UTF16 };

    struct StreamLineData {
        size_t id;
        size_t startAt;
        int32_t length;
    };

    using StreamLineData_vec = std::vector<StreamLineData>;

    struct StreamConfig {
        std::filesystem::path filePath;
        int chunkSize{32_kb};
        FileEncoding encoding{FileEncoding::UTF8};
    };

} // namespace omlog::defs
