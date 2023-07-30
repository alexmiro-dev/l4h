#pragma once

#include "defs.hpp"
#include "utils.h"
#include "StreamBuffers.hpp"
#include "coroutine_utils.hpp"

#include <fstream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include <iostream>

namespace omlog::stream {

using namespace defs;


template <typename T>
concept StreamObservable = requires(T stream) {
    { stream.onData() } -> std::same_as<void>;
    { stream.onPaused()} -> std::same_as<void>;
    { stream.onResumed()} -> std::same_as<void>;
    { stream.onStopped()} -> std::same_as<void>;
    { stream.onReset()} -> std::same_as<void>;
    { stream.onEof()} -> std::same_as<void>;
};

template <StreamObservable Observer>
class TextStreamReader {
public:
    explicit TextStreamReader(StreamConfig&& config) : config_{std::move(config)} {}

    TextStreamReader() = delete;

    ~TextStreamReader() { 
        if (readingThread_.joinable()) {
            readingThread_.request_stop();
        }
    }

    void start() {
        if (readingThread_.joinable()) {
            return; // It is running;
        }
        load();

        readingThread_ = std::jthread([this] (std::stop_token stopToken) {
            startReading(stopToken);
        });
    }

    void pause() {
        if (!isPaused_.load()) {
            isPaused_ = true;
        }
    }

    void resume() {
        if (isPaused_.load()) {
            isPaused_ = false;

            std::unique_lock<std::mutex> lock{mtx_};
            cv_.notify_one();
        }
    }

    void stop() {
        if (readingThread_.joinable()) {
            readingThread_.request_stop();
        }
    }
    
    void reset() {

    }

    void waitToFinish() {
        if (readingThread_.joinable()) {
            readingThread_.join();
        }
    }

private:
    void load() {
        if (auto fileSizeOpt = utils::fileSize(config_.filePath); !fileSizeOpt) {
            // Throw
        }
        std::ifstream file{config_.filePath.string()};

        if (!file) {
            // Throw
        }
        fileStream_ = std::move(file);
    }

    coro::Generator<std::string> readChunks() {
        static std::vector<char> buffer(config_.chunkSize);

        while (!fileStream_.eof()) {
            fileStream_.read(buffer.data(), config_.chunkSize);

            auto bytes = fileStream_.gcount();

            std::string s(buffer.begin(), buffer.begin() + bytes);
            co_yield s;
        }
    }

    void startReading(std::stop_token stopToken) {
        for (auto const& valueStr : readChunks()) {
            if (stopToken.stop_requested()) { 
                break; 
            }
            while (isPaused_.load()) {
                if (stopToken.stop_requested()) { break; }

                {
                    std::unique_lock<std::mutex> lock{mtx_};
                    
                    if (cv_.wait_for(lock, std::chrono::seconds(1), [isPaused = isPaused_.load()] { return !isPaused; })) {
                        // The condition has been met: it is not paused anymore.
                        break;
                    } else {
                        // The wait timed out or spurious wakeup occurred
                        std::cout << "\nPAUSED" << std::endl;
                        continue;
                    }
                }
            }
            std::cout << valueStr;
        }
    }


    StreamConfig config_;
    std::ifstream fileStream_;
    std::jthread readingThread_;
    std::atomic_bool isPaused_{false};
    std::condition_variable cv_;
    std::mutex mtx_;
};

} // omlog::stream
