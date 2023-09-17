
#pragma once

#include <concepts>
#include <string_view>
#include <fmt/core.h>

namespace omlog::header {

// Reference: spdlog/pattern_formatter-inl.h

template <typename T>
concept HeaderEntity = requires(T entity) {
    {entity.to_regex()} -> std::same_as<std::string_view>;
};

enum class DateFormat { YYYY_MM_DD, DD_MM_YYYY };

class Date {
private:
    static constexpr std::string_view YYYY_MM_DD = R"#((\d{4}[-/]\d{2}[-/]\d{2}))#";
    static constexpr std::string_view DD_MM_YYYY = R"#((\d{2}[-/]\d{2}[-/]\d{4}))#";
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

struct TimeDivision {
    enum class Unity {None, Milliseconds, Microseconds, Nanoseconds };

    Unity unity{Unity::None};
    uint8_t digits{3U};
    char separator{'\0'};
};

// \d{2}:\d{2}:\d{2}\.\d{3}
class Time {
public:
    explicit Time(TimeDivision&& time_division) 
        : division_{std::move(time_division)} {}

    std::string_view to_regex() const {
        static constexpr auto hhmmss = "\\d{2}:\\d{2}:\\d{2}";

        if (division_.unity != TimeDivision::Unity::None) {
            const std::string sep = division_.separator == '.' ? "\\." : std::string{division_.separator};
            const std::string pattern = fmt::format("({}{}\\d{{}})", hhmmss, sep, division_.digits);
            return pattern;
        }
        return hhmmss;
    }
private:
    TimeDivision division_;
};

class LoggerName {
public:
    consteval LoggerName() {}
    std::string_view to_regex() const { return "(.*?)"; }
};

enum class LogLevel {Trace, Debug, Info, Warn, Err, Critical};

class Level {
public:
    consteval Level() {}

    std::string_view to_regex() const { return "(.*?)"; }
};

class ThreadId {
public:
    consteval ThreadId() {}

    // The thread Id could be represented by a hexadecimal value as well
    std::string_view to_regex() const { return "([A-Fa-f0-9]+)"; }
};

class Pid {
public:
    consteval Pid() {}

    std::string_view to_regex() const { return "([0-9]+)"; }
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

    std::string_view to_regex() const { return "(.*?)"; }
};

class SourceLine {
public:
    consteval SourceLine() {}

    std::string_view to_regex() const { return "([0-9]+)"; }
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
                regex_ += escape_if_regex_symbol(pattern.front());
                pattern.remove_prefix(1);
            }
        }
    }

    std::string escape_if_regex_symbol(char c) const {
        if (regex_metachars_.find_first_of(c) != std::string_view::npos) {
            std::string ret{"\\"};
            ret += c;
            return ret;
        }
        return {c};
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

    constexpr static std::string_view regex_metachars_ = ".*+?|[]()^$\\";
    constexpr static std::string_view place_holder_ = "{}";
    std::string regex_;
};

} // namespace omlog::headerclass ScopeMessageBegin {