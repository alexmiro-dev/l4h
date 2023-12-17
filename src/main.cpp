

#include "LogStreamReader.hpp"

#include <regex>

using namespace l4h;

using namespace std::chrono_literals;
using namespace std::literals;

int main()
{
    defs::StreamConfig config;
    config.file_path = "/home/miro/dev/vm-desktop.log";

    TimeFraction time_division{TimeFraction::Unity::Nanoseconds, 3U, '.'};
    auto line_parser = std::make_shared<LineParser>("[{} {}] [{}] [{}] {}"sv
                                    , Date{DateFormat::YYYY_MM_DD}
                                    , Time{std::move(time_division)}
                                    , LoggerName{}
                                    , Level{}
                                    , Message{});

    const std::string line = "[2023-07-19 12:55:48.691] [app-desktop] [debug] The message";
    auto line_parts = line_parser->deserialize(line);

    config.line_parser = line_parser;


    LogStreamReader logStream{std::move(config)};


    // logStream.start();

    /*
        std::this_thread::sleep_for(1s);

        logStream.pause();
        std::this_thread::sleep_for(3s);
        logStream.resume();

    */
    logStream.waitToFinish();

    return 0;
}
