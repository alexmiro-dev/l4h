

#include "LogStreamReader.hpp"

using namespace omlog;
namespace h = omlog::header;

using namespace std::chrono_literals;
using namespace std::literals;

int main()
{
    defs::StreamConfig config;
    config.file_path = "/home/miro/dev/vm-desktop.log";

    // [2023-07-19 12:55:48.691 voicemod-desktop debug] The message
    // [YYYY-MM-DD HH:mm:48.691 voicemod-desktop debug] The message

    // "YYYY-MM-DD"_regex_date << "HH:MM:SS."

    LogStreamReader logStream{std::move(config)};


    h::HeaderPattern header = h::HeaderPattern{"[{} {}] [{}] [{}] {}"sv
                                    , h::Date{h::DateFormat::YYYY_MM_DD}
                                    , h::Time{}
                                    , h::LoggerName{}
                                    , h::Level{}
                                    , h::Message{}};

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
