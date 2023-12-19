
#pragma once

#include <unordered_map>
#include <set>
#include <ranges>

#include "LogRecord.hpp"

namespace l4h {

class LevelMonitor {
    struct Cache {
        defs::LogLevel level{defs::LogLevel::Unknown};  // The last recorded level having LogRecordType::HeaderAndMessage
        std::set<defs::line_uid_t> child_lines;
    };
public:
    void on_update(LogRecord record) {
        if (record.type() == defs::LogRecordType::MessagePart && cache_.level != defs::LogLevel::Unknown) {
            cache_.child_lines.emplace(record.uid());
            return;
        }
        flush_cache();
        auto const level = record.get<Level>();

        if (cache_.level = level->type(); !levels_.contains(cache_.level)) {
            levels_.emplace(std::make_pair(cache_.level, std::set{record.uid()}));
        } else {
            levels_[cache_.level].insert(record.uid());
        }
    }

private:
    void flush_cache() {
        if (cache_.level != defs::LogLevel::Unknown
            && !cache_.child_lines.empty()
            && levels_.contains(cache_.level)) {

            auto& level_childs = levels_[cache_.level];
            std::ranges::copy(cache_.child_lines, std::inserter(level_childs, level_childs.end()));
        }
        cache_.level = defs::LogLevel::Unknown;
        cache_.child_lines.clear();
    }

    Cache cache_;
    std::unordered_map<defs::LogLevel, std::set<defs::line_uid_t>> levels_;
};

}//  namespace l4h {
