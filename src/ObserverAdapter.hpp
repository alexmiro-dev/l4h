
#pragma once

#include <functional>

namespace l4h {

// Observer pattern using value semantics (std::function)
template <typename Subject, typename StateChangedTag>
class ObserverAdapter {
public:
    using OnUpdate = std::function<void(Subject const&, StateChangedTag)>;

    explicit ObserverAdapter(OnUpdate on_update) : on_update_{std::move(on_update)} {}

    void update(Subject const& subject, StateChangedTag tag) {

        // TODO: should run in a task to avoid deadlocks (execution timeout)
        on_update_(subject, tag);
    }
private:
    OnUpdate on_update_;
};

} // namespace l4h
