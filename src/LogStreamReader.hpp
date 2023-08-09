
#pragma once

#include "TextStreamReader.hpp"
#include "StreamBuffers.hpp"
#include "ReactiveStaticQueue.hpp"

namespace omlog {

using namespace omlog::defs;

/**
 *
 */
class LogStreamReaderImpl {
public:
    LogStreamReaderImpl() {
        raw_data_queue_.register_reaction_on_poped([this] (std::string&& data) {
            on_reactive_queue_data(std::move(data));
        });
    }

    ~LogStreamReaderImpl() {}

    void onData(std::string&& data) {
        raw_data_queue_.push(std::move(data));
    }
    void onPaused() {}
    void onResumed() {}
    void onStopped() {}
    void onEof() {}

private:
    void on_reactive_queue_data(std::string&& data) {

    }

    stream::StreamBuffer streamBuffer_;
    ds::ReactiveStaticQueue<std::string, 100> raw_data_queue_;
};

using LogStreamReader = stream::TextStreamReader<LogStreamReaderImpl>;


} // namespace omlog
