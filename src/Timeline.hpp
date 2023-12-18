
#pragma once

#include "LogRecord.hpp"

#include <chrono>
#include <map>
#include <vector>

namespace l4h {

using time_duration_key_t = std::chrono::duration<double>;

/** Responsible for windowing the log entries on a timeline
 *
 */
class Timeline {
public:
    struct Entry {
        defs::line_uid_t line_uid;
        Date date;
        Time time;
        std::optional<Level> level;
        std::optional<ThreadId> thread_id;
        std::optional<LoggerName> logger_name;
        std::vector<defs::line_uid_t> child_lines; // Subsequent lines without header: defs::LogRecordType::MessagePart
    };

    void on_update(LogRecord record) {
        if (record.type() == defs::LogRecordType::MessagePart) {
            if (time_entries_.contains(last_entry_key_)) {
                time_entries_[last_entry_key_].child_lines.push_back(record.uid());
            }
            // TODO: Report this anomaly.
            return;
        }
        auto const date_opt = record.get<Date>();
        auto const time_opt = record.get<Time>();

        if (!date_opt || !time_opt) {
            // Something went wrong with this record
            //
            // TODO: log me
            return;
        }
        Entry entry;
        entry.date = date_opt->clone();
        entry.time = time_opt->clone();

        if (auto const thread_opt = record.get<ThreadId>(); thread_opt) {
            entry.thread_id = thread_opt->clone();
        }
        if (auto const logger_name = record.get<LoggerName>(); logger_name) {
            entry.logger_name = logger_name->clone();
        }
        if (auto const level = record.get<Level>(); level) {
            entry.level = level->clone();
        }
        last_entry_key_ = entry.time.duration();
        time_entries_.try_emplace(last_entry_key_, std::move(entry));

        // TODO: notify
    }

private:
    time_duration_key_t last_entry_key_;
    std::map<time_duration_key_t, Entry> time_entries_;
};

} // namespace l4h
