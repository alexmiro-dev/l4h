#pragma once

#include "definitions.hpp"
#include "utils.h"
#include "StreamBuffers.hpp"
#include "coroutine_utils.hpp"

#include <fstream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include <iostream>

namespace l4h {

template <typename T>
concept StreamObservable = requires(T stream, std::string data, double percentage, defs::StreamConfig const& config) {
    { stream.config(std::forward<defs::StreamConfig const&>(config)) } -> std::same_as<void>;
    { stream.on_data(std::forward<std::string>(data)) } -> std::same_as<void>;
    { stream.on_paused()} -> std::same_as<void>;
    { stream.on_resumed()} -> std::same_as<void>;
    { stream.on_stopped()} -> std::same_as<void>;
    { stream.on_eof()} -> std::same_as<void>;
    { stream.on_read_percentage(std::forward<double>(percentage))} -> std::same_as<void>;
};

template <StreamObservable Implementation>
class TextStreamReader {
public:
    explicit TextStreamReader(defs::StreamConfig&& config) : config_{std::move(config)} {
        implementation_.config(config_);
    }

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
        if (auto file_size_opt = utils::fileSize(config_.file_path); file_size_opt) {
            file_size_ = file_size_opt.value();
        }
        std::ifstream file{config_.file_path.string()};

        if (!file) {
            // Throw
        }
        file_stream_ = std::move(file);
    }

    coro::Generator<std::string> read_chunks() {
        static std::vector<char> buffer(config_.chunk_size);

        double total_bytes_read = 0.0;

        while (!file_stream_.eof()) {
            file_stream_.read(buffer.data(), config_.chunk_size);
            auto const bytes = file_stream_.gcount();
            std::string s(buffer.begin(), buffer.begin() + bytes);
            total_bytes_read += bytes;
            implementation_.on_read_percentage((total_bytes_read / file_size_) * 100.0);
            co_yield s;
        }
    }

    void start_reading(std::stop_token stop_token) {
        for (auto&& valueStr : read_chunks()) {
            implementation_.on_data(valueStr);

            while (is_paused_.load() && !stop_token.stop_requested()) {
                std::unique_lock lock{mtx_};

                if (cv_.wait_for(lock, std::chrono::milliseconds(300), [is_paused = is_paused_.load()]
                                 { return !is_paused; })) {
                    break; // The condition has been met: it is not paused anymore.

                } // else: The wait timed out or spurious wakeup occurred
            }
            if (stop_token.stop_requested()) { break; }
        }
        implementation_.on_eof();
    }


    defs::StreamConfig config_;
    std::ifstream file_stream_;
    std::size_t file_size_;
    std::jthread reading_thr_;
    std::atomic_bool is_paused_{false};
    std::condition_variable cv_;
    std::mutex mtx_;
    Implementation implementation_;
};

} // l4h
