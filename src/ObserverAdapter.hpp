
#pragma once

#include <functional>
#include <thread>
#include <atomic>

namespace l4h {

// Observer pattern using value semantics (std::function)
//
template <typename Subject>
class ObserverAdapter {
public:
    using OnUpdate = std::function<void(Subject)>;

private:
    class ConcurrentUpdater {
    public:
        explicit ConcurrentUpdater(OnUpdate on_update) : update_{std::move(on_update)} {}

        void run(Subject subject) {
            std::jthread([this, &subject] {
                while (is_running_.test_and_set()) {
                    std::this_thread::yield();
                }
                update_(subject);

                is_running_.clear();
                is_running_.notify_one();
            });
        }

    private:
        std::atomic_flag is_running_{ATOMIC_FLAG_INIT};
        std::jthread thread_;
        OnUpdate update_;
    };

public:
    explicit ObserverAdapter(OnUpdate on_update) : updater_{std::move(on_update)} {}

    // Runs the callback concurrently to avoid blocking the caller of this function.
    void update(Subject subject) {
        updater_.run(subject);
    }

private:
    ConcurrentUpdater updater_;
};

} // namespace l4h
