
#pragma once

#include <optional>
#include <filesystem>

namespace omlog::utils {

    std::optional<size_t> fileSize(const std::filesystem::path& file_path) {
        if (!std::filesystem::exists(file_path)) {
            return std::nullopt;
        }
        return std::filesystem::file_size(file_path);
    }

} // namespace omlog::utils
