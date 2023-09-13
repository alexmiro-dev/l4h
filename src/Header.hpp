
#pragma once

#include <concepts>
#include <string_view>

namespace omlog::header {

// Reference: spdlog/pattern_formatter-inl.h

template <typename T>
concept HeaderEntity = requires(T entity) {
    {entity.to_regex()} -> std::same_as<std::string_view>;
};

enum class DateFormat { YYYY_MM_DD, DD_MM_YYYY };

class Date {
private:
    static constexpr std::string_view YYYY_MM_DD = R"(\d{4}[-/]\d{2}[-/]\d{2})";
    static constexpr std::string_view DD_MM_YYYY = R"(\d{2}[-/]\d{2}[-/]\d{4})";
public:

    consteval explicit Date(DateFormat f) : format_{f} { }

    std::string_view to_regex() const {
        switch (format_) {
            case DateFormat::DD_MM_YYYY: return DD_MM_YYYY;
            case DateFormat::YYYY_MM_DD: return YYYY_MM_DD;
        }
        return "UNKNOWN_DATE_FORMAT";   // TODO: Is there a better way to return a valid value? Default?
    }

private:
    DateFormat format_;
};

class Time {
public:
    consteval Time() {}

    std::string_view to_regex() const {
        return "";
    }
};

class LoggerName {
public:
    consteval LoggerName() {}

    std::string_view to_regex() const {
        return "";
    }
};

class Level {
public:
    consteval Level() {}

    std::string_view to_regex() const {
        return "";
    }
};

class ThreadId {
public:
    consteval ThreadId() {}

    std::string_view to_regex() const {
        return "";
    }
};

class Pid {
public:
    consteval Pid() {}

    std::string_view to_regex() const {
        return "";
    }
};

class Source {
public:
    consteval Source() {}

    std::string_view to_regex() const {
        return "";
    }
};

class SourceFuncName {
public:
    consteval SourceFuncName() {}

    std::string_view to_regex() const {
        return "";
    }
};

class SourceLine {
public:
    consteval SourceLine() {}

    std::string_view to_regex() const {
        return "";
    }
};

class Message {
public:
    consteval Message() {}

    std::string_view to_regex() const {
        return "";
    }
};

class ScopeMessageBegin {
public:
    consteval ScopeMessageBegin() {}

    std::string_view to_regex() const {
        return "";
    }
};

class ScopeMessageEnd {
public:
    consteval ScopeMessageEnd() {}

    std::string_view to_regex() const {
        return "";
    }
};

class HeaderPattern {
public:
    template <HeaderEntity... PatternArgs>
    explicit HeaderPattern(std::string_view pattern, PatternArgs... args) {

        if (int num_place_holders = count_place_holders(pattern); num_place_holders == sizeof...(PatternArgs)) {
            build_pattern(pattern, args...);
        } else {
            // TODO: throw
        }
    }

    // Receives a line and try to return the entities from it
    void deserialize(std::string_view log_line) {

    };

private:
    template <typename... Entities>
    void build_pattern(std::string_view pattern, const auto& entity, Entities... entities) { 
        if (pattern.empty()) { return; }

        while (!pattern.empty()) {
            if (pattern.starts_with(place_holder_)) {
                regex_.append(entity.to_regex());
                pattern.remove_prefix(2);
                build_pattern(pattern, entities...);
                break;
            } else {
                regex_ += pattern.front();
                pattern.remove_prefix(1);
            }
        }
    }

    //Base case for the recursive function (no arguments left to process)
    void build_pattern([[maybe_unused]] std::string_view pattern) const {}

    int count_place_holders(std::string_view pattern, int count = 0) const {
        if (pattern.empty()) { return count; }

        if (pattern.starts_with(place_holder_)) {
            return count_place_holders(pattern.substr(place_holder_.size()), count + 1);
        } else {
            return count_place_holders(pattern.substr(1), count);
        }
    }

    constexpr static std::string_view place_holder_ = "{}";
    std::string regex_;
};

} // namespace omlog::headerclass ScopeMessageBegin {