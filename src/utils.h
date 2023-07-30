
#pragma once

#include <optional>
#include <filesystem>

namespace omlog::utils {

    std::optional<size_t> fileSize(const std::filesystem::path& filePath) {
        if (!std::filesystem::exists(filePath)) {
            return std::nullopt;
        }
        return std::filesystem::file_size(filePath);
    }

} // namespace omlog::utils
