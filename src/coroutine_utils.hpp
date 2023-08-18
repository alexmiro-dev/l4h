
#pragma once

#include <cassert>
#include <coroutine>

namespace omlog::coro {

template <typename T>
class [[nodiscard]] Generator {
public:
    Generator(auto handle) : handle_{handle} {}
    ~Generator() { if(handle_) handle_.destroy(); }

    struct promise_type {
        T value;                //  Last value from co_yield

        auto yield_value(T val) {    // reaction to co_yield
            value = std::move(val);
            return std::suspend_always{};
        }
        auto get_return_object() { return std::coroutine_handle<promise_type>::from_promise(*this); }
        auto initial_suspend() noexcept { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    Generator(Generator const&) = delete;
    Generator& operator=(Generator const&) = delete;

    // API to resume the coroutine and access its values
    // - iterator interface with begin() and end()
    struct iterator {
        std::coroutine_handle<promise_type> handle_;
        
        explicit iterator(auto handle) : handle_{handle} {}

        void getNext() {
            if (handle_) {
                if (handle_.resume(); handle_.done()) {
                    handle_ = nullptr;
                }
            }
        }

        auto operator*() const {
            assert(handle_ != nullptr);
            return handle_.promise().value;
        }

        iterator operator++() {
            getNext();
            return *this;
        }

        bool operator==(iterator const&) const = default;
    };

    iterator begin() const {
        if (!handle_ || handle_.done()) {
            return iterator{nullptr};
        }
        iterator itor{handle_};
        itor.getNext();
        return itor;
    }

    iterator end() const {
        return iterator{nullptr};
    }

private:
    std::coroutine_handle<promise_type> handle_;    
};

} // namespace omlog::coro
