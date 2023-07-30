
#pragma once

#include "TextStreamReader.hpp"

namespace omlog {

using namespace omlog::defs;

/**
 *
 */
class LogStreamReaderImpl {
public:
    LogStreamReaderImpl() {}
    ~LogStreamReaderImpl() {}

    void onData() {}
    void onPaused() {}
    void onResumed() {}
    void onStopped() {}
    void onReset() {}
    void onEof() {}

private:
};

using LogStreamReader = stream::TextStreamReader<LogStreamReaderImpl>;


} // namespace omlog
