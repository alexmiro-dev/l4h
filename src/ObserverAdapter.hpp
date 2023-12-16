
#pragma once

#include <functional>

namespace l4h {

// Observer pattern using value semantics (std::function)
//
template <typename Subject>
class ObserverAdapter {
public:
    using OnUpdate = std::function<void(Subject const&)>;

    explicit ObserverAdapter(OnUpdate on_update) : on_update_{std::move(on_update)} {}

    void update(Subject const& subject) {
        on_update_(subject);
    }

    void update_async(Subject const& subject) {
        // TODO: should run in a task to avoid deadlocks (execution timeout)
        on_update_(subject);
    }

private:
    OnUpdate on_update_;
};

} // namespace l4h
