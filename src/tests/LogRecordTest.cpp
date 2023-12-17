#define CONFIG_CATCH_MAIN

#include <catch2/catch_test_macros.hpp>

#include "LogRecord.hpp"

using namespace l4h;
using namespace std::literals;

TEST_CASE("LogRecordTest", "[convert]") {

    TimeFraction time_division{TimeFraction::Unity::Nanoseconds, 3U, '.'};
    auto line_parser = std::make_shared<LineParser>("[{} {}] [{}] [{}] [{}] {}"sv, Date{DateFormat::YYYY_MM_DD},
                                                    Time{std::move(time_division)}, ThreadId{}, LoggerName{}, Level{},
                                                    Message{});

    const std::string line = "[2023-07-19 12:55:48.691] [533123] [app-desktop] [debug] The message";
    auto line_parts = line_parser->deserialize(line);

    REQUIRE(line_parts.size() == 6);

    LogRecord record;

    record.set_type(defs::LogRecordType::HeaderAndMessage);
    record.set_uid(defs::line_uid_t{1U});
    record.set_tokens(line_parts);

    auto time_opt = record.get<Time>();
    auto time = time_opt.value();
}
