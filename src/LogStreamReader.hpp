
#pragma once

#include "TextStreamReader.hpp"
#include "StreamBuffers.hpp"
#include "ReactiveStaticQueue.hpp"

#include <mutex>
#include <condition_variable>

#include <iostream>

namespace omlog {


/**
 *
 */
class LogStreamReaderImpl {
public:
    LogStreamReaderImpl() {
        using namespace omlog::defs;

        raw_data_queue_.register_reaction_on_popped([this] (std::string data) {
            on_reactive_queue_data(std::move(data));
        });

        data_.line_vec.reserve(2_mb);
    }

    ~LogStreamReaderImpl() = default;

    void on_data(std::string data) { raw_data_queue_.push(std::move(data)); }

    void on_paused() {}
    void on_resumed() {}
    void on_stopped() {}

    void on_eof()
    {
        raw_data_queue_.consume_all_and_finish();

        std::unique_lock locker{queue_data_mtx_};
        queue_data_finished_cv_.wait(locker, [this] { return processed_queue_data_.load(); });

        if (!data_.line_vec.empty())
        {
            data_.info.line = std::string{data_.line_vec.begin(), data_.line_vec.end()};
            lines_.emplace_back(data_.info);
        }
        std::ofstream out{"/home/miro/dev/full-result.txt"};

        for (auto const &line : lines_)
        {
            out << line.line;
        }
        out.close();
    }

private:
    struct DataHelper {
        int32_t line_id{1};
        std::vector<char> line_vec;
        long long cursor_pos{0LL};
        defs::StreamLineData info;
    };

    void on_reactive_queue_data(std::string data) {
        
        std::unique_lock locker{queue_data_mtx_};
        processed_queue_data_ = false;
        locker.unlock();

        auto const text = std::move(data);

        for (char c : text) {
            data_.line_vec.push_back(c);

            if (c == defs::g_new_line) {
                data_.info.id = data_.line_id++;
                data_.info.line = std::string{data_.line_vec.begin(), data_.line_vec.end()};
                lines_.push_back(data_.info);

                data_.line_vec.clear();
                data_.info.start_pos = data_.cursor_pos + 1;
            }
            ++data_.cursor_pos;
        }
        locker.lock();
        processed_queue_data_ = true;
        locker.unlock();
        queue_data_finished_cv_.notify_one();
    }


    ds::ReactiveStaticQueue<std::string, 16384> raw_data_queue_; // power of two
    std::vector<defs::StreamLineData> lines_;
    DataHelper data_;
    std::mutex queue_data_mtx_;
    std::atomic_bool processed_queue_data_{false};
    std::condition_variable queue_data_finished_cv_;
};

using LogStreamReader = stream::TextStreamReader<LogStreamReaderImpl>;

} // namespace omlog
