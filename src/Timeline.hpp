
#pragma once

#include "LogRecord.hpp"

#include <chrono>
#include <map>
#include <vector>

namespace l4h {

/** Responsible for windowing the log entries on a timeline
 *
 */
class Timeline {
public:

    void on_update(LogRecord record) {
        if (record.type() == defs::LogRecordType::MessagePart) {
            return;
        }
        // get the time
        auto const time = record.get<Time>();

        // insert it to the map by the second

        // notify
    }

private:
    defs::line_uid_t last_non_message_part_entry_{defs::g_line_no_uid};
    std::map<std::chrono::time_point<std::chrono::system_clock>, std::vector<defs::line_uid_t>> time_entries_;
};

}
