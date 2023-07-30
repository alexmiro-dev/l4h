

#include "LogStreamReader.hpp"

using namespace omlog;

#include <thread>

using namespace std::chrono_literals;

int main()
{
    stream::StreamConfig config;
    config.filePath = "/home/miro/dev/vm-desktop.log";

    LogStreamReader logStream{std::move(config)};

    logStream.start();

    std::this_thread::sleep_for(1s);

    logStream.pause();
    std::this_thread::sleep_for(3s);
    logStream.resume();

    logStream.waitToFinish();

    return 0;
}
