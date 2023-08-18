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

template <typename T>
concept StreamObservable = requires(T stream, std::string data) {
    { stream.on_data(std::forward<std::string>(data)) } -> std::same_as<void>;
    { stream.on_paused()} -> std::same_as<void>;
    { stream.on_resumed()} -> std::same_as<void>;
    { stream.on_stopped()} -> std::same_as<void>;
    { stream.on_eof()} -> std::same_as<void>;
};

template <StreamObservable Observer>
class TextStreamReader {
public:
    explicit TextStreamReader(defs::StreamConfig&& config) : config_{std::move(config)} {}

    TextStreamReader() = delete;
    TextStreamReader(TextStreamReader const&) = delete;

    ~TextStreamReader() { 
        if (reading_thr_.joinable()) {
            reading_thr_.request_stop();
        }
    }

    void start() {
        if (reading_thr_.joinable()) {
            return; // It is running;
        }
        load();

        reading_thr_ = std::jthread([this] (std::stop_token stop_token) {
            start_reading(stop_token);
        });
    }

    void pause() {
        if (!is_paused_.load()) {
            is_paused_ = true;
        }
    }

    void resume() {
        if (is_paused_.load()) {
            is_paused_ = false;

            std::unique_lock lock{mtx_};
            cv_.notify_one();
        }
    }

    void stop() {
        if (reading_thr_.joinable()) {
            reading_thr_.request_stop();
        }
    }
    
    void waitToFinish() {
        if (reading_thr_.joinable()) {
            reading_thr_.join();
        }
    }

private:
    void load() {
        if (auto file_size_opt = utils::fileSize(config_.file_path); !file_size_opt) {
            // Throw
        }
        std::ifstream file{config_.file_path.string()};

        if (!file) {
            // Throw
        }
        file_stream_ = std::move(file);
    }

    coro::Generator<std::string> read_chunks() {
        static std::vector<char> buffer(config_.chunkSize);

        while (!file_stream_.eof()) {
            file_stream_.read(buffer.data(), config_.chunkSize);

            auto bytes = file_stream_.gcount();

            std::string s(buffer.begin(), buffer.begin() + bytes);
            co_yield s;
        }
    }

    void start_reading(std::stop_token stop_token) {
        for (auto valueStr : read_chunks()) {
            if (stop_token.stop_requested()) { 
                break; 
            }
            while (is_paused_.load()) {
                if (stop_token.stop_requested()) {
                    break;
                }

                std::unique_lock lock{mtx_};

                if (cv_.wait_for(lock, std::chrono::seconds(1), [is_paused = is_paused_.load()]
                                 { return !is_paused; })) {
                    // The condition has been met: it is not paused anymore.
                    break;
                } else {
                    // The wait timed out or spurious wakeup occurred
                    continue;
                }
            }
            observer_.on_data(valueStr);
            // std::cout << valueStr;
        }
        observer_.on_eof();
    }


    defs::StreamConfig config_;
    std::ifstream file_stream_;
    std::jthread reading_thr_;
    std::atomic_bool is_paused_{false};
    std::condition_variable cv_;
    std::mutex mtx_;
    Observer observer_;
};

} // omlog::stream
