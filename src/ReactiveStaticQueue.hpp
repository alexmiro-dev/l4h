
#pragma once

#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <functional>
#include <thread>

namespace omlog::ds {

using OnQueuePopedFunc = std::function<void(std::string&&)>;

template <typename T, std::size_t TCapacity>
class ReactiveStaticQueue final {
public:
    static_assert(TCapacity > 0, "Capacity must be greater than zero");

    using LockGuard = std::lock_guard<std::mutex>;
    using UniqueLock = std::unique_lock<std::mutex>;
    using ScopedLock = std::scoped_lock<std::mutex>;

    /**
     * @brief
     * @param
     */
    ReactiveStaticQueue() {
        consumer_thr_ = std::jthread([this] (std::stop_token stop_token) {
            pop(stop_token);
        });
    }

    ~ReactiveStaticQueue() {
        if (consumer_thr_.joinable()) {
            consumer_thr_.request_stop();
        }
    }

    /**
     * @brief Deleted constructors
     */
    ReactiveStaticQueue(ReactiveStaticQueue const&)             = delete;
    ReactiveStaticQueue& operator=(ReactiveStaticQueue const&)  = delete;
    ReactiveStaticQueue(ReactiveStaticQueue&&)                  = delete;
    ReactiveStaticQueue& operator=(ReactiveStaticQueue&&)       = delete;

    /**
     * @brief size
     * @return
     */
    std::size_t size() {
        decltype(TCapacity) size = 0u;
        {
            ScopedLock lock{size_mtx_};
            size = size_;
        }
        return size;
    }

    /**
     * @brief push
     * @param newItem
     */
    void push(T&& newItem) {
        UniqueLock lock{size_mtx_};
        size_changed_cv_.wait(lock, [&]() { return size_ < TCapacity; });

        queue_[front_] = std::move(newItem);

        advance(front_);
        size_++;
        size_changed_cv_.notify_all();
    }

    void register_reaction_on_poped(OnQueuePopedFunc f) {
        consume_ = f;
    }

private:
    constexpr void advance(std::size_t& index) {
        //    The purpose of the modulus operation in this context is to ensure that the index front_ 
        // remains within the valid range of the array queue_. Since front_ represents the index of 
        // the next available slot in the queue, it needs to wrap around when it reaches the end of 
        // the array to avoid accessing out-of-bounds memory.
        //    When front_ reaches the end of the array (index TCapacity - 1), the modulus operation wraps it 
        // back to the beginning (index 0), simulating the circular behavior of the buffer.

        constexpr bool isPowerOfTwo = (TCapacity & (TCapacity - 1)) == 0;

        if constexpr (isPowerOfTwo) {
            // If TCapacity is power of two THEN `x % TCapacity` is equivalent `x & (TCapacity - 1)` 
            // This operation has a better performace than the modulus operation (% TCapacity)
            index = (index + 1u) & (TCapacity - 1);
        } else {
            index = (index + 1u) % TCapacity;
        }
    }

    /**
     * @brief pop
     * @return
     */
    void pop(std::stop_token stop_token)
    {
        while (!stop_token.stop_requested())
        {
            {
                UniqueLock lock{size_mtx_};
                if (size_changed_cv_.wait_for(lock, std::chrono::milliseconds(100), [&]()
                                      { return size_ > 0u;  })) {
                    ; // do nothing
                } else {
                    // The wait timed out or spurious wakeup occurred
                    continue;
                }
            }
            consume_(std::move(queue_[back_]));
            advance(back_);

            UniqueLock lock{size_mtx_};
            size_--;
            size_changed_cv_.notify_all();
            lock.unlock();
        }
    }

    decltype(TCapacity) size_{0u};
    decltype(TCapacity) front_{0u};
    decltype(TCapacity) back_{0u};
    std::array<T, TCapacity> queue_;
    std::mutex size_mtx_;
    std::condition_variable size_changed_cv_;
    OnQueuePopedFunc consume_;
    std::jthread consumer_thr_;
};

} // omlog::ds
