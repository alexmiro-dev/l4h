

#include "LogStreamReader.hpp"

#include <regex>

using namespace omlog;
namespace h = omlog::header;

using namespace std::chrono_literals;
using namespace std::literals;

int main()
{
    defs::StreamConfig config;
    config.file_path = "/home/miro/dev/vm-desktop.log";

    h::TimeDivision time_division{h::TimeDivision::Unity::Nanoseconds, 3U, '.'};
    auto header = h::HeaderPattern{"[{} {}] [{}] [{}] {}"sv
                                    , h::Date{h::DateFormat::YYYY_MM_DD}
                                    , h::Time{std::move(time_division)}
                                    , h::LoggerName{}
                                    , h::Level{}
                                    , h::Message{}};

    const std::string line = "[2023-07-19 12:55:48.691] [voicemod-desktop] [debug] The message";
    header.deserialize(line);

    config.header_pattern = std::move(header);


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
