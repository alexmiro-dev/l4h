
#pragma once

#include <fmt/core.h>

#include <concepts>
#include <string_view>
#include <variant>
#include <vector>
#include <deque>
#include <regex>
#include <algorithm>


namespace omlog::header {

// Reference: spdlog/pattern_formatter-inl.h

template <typename T>
concept HeaderEntity = requires(T entity, std::string_view value_sv) {
    {entity.to_regex()} -> std::same_as<std::string_view>;
    {std::as_const(entity).clone()} -> std::same_as<T>;
    {entity.set_value(value_sv)};
    {std::as_const(entity).value()} -> std::same_as<std::string const&>;
};

enum class DateFormat { YYYY_MM_DD, DD_MM_YYYY };

class Date {
private:
    static constexpr std::string_view YYYY_MM_DD = R"#((\d{4}[-/.]\d{1,2}[-/.]\d{1,2}))#";
    static constexpr std::string_view DD_MM_YYYY = R"#((\d{1,2}[-\/.]\d{1,2}[-\/.]\d{4}))#";
public:
    explicit Date(DateFormat f) : format_{f} { }

    std::string_view to_regex() {
        switch (format_) {
            case DateFormat::DD_MM_YYYY: return DD_MM_YYYY;
            case DateFormat::YYYY_MM_DD: return YYYY_MM_DD;
        }
        return "UNKNOWN_DATE_FORMAT";   // TODO: Is there a better way to return a valid value? Default?
    }

    Date clone() const { return {*this}; }

    // TODO: convert this value to a tm structure 
    void set_value(std::string_view value) { value_ = value; }

    const std::string& value() const { return value_;}

private:
    DateFormat format_;
    std::string value_;
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

    std::string_view to_regex() {
        static constexpr auto hhmmss = R"#(\d{2}:\d{2}:\d{2})#";

        if (division_.unity != TimeDivision::Unity::None) {
            const std::string sep = division_.separator == '.' ? "\\." : std::string{division_.separator};
            pattern_ = fmt::format(R"#(({}{}\d{{{}}}))#", hhmmss, sep, division_.digits);
            return pattern_;
        }
        return hhmmss;
    }
    Time clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    TimeDivision division_;
    std::string pattern_;
    std::string value_;
};

class LoggerName {
public:
    LoggerName() = default;
    std::string_view to_regex() { return R"#((.*?))#"; }
    LoggerName clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

enum class LogLevel {Trace, Debug, Info, Warn, Err, Critical};

class Level {
public:
    Level() = default;
    std::string_view to_regex() { return R"#((.*?))#"; }
    Level clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class ThreadId {
public:
    ThreadId() = default;

    // The thread Id could be represented by a hexadecimal value as well
    std::string_view to_regex() { return R"#(([A-Fa-f0-9]+))#"; }
    ThreadId clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class Pid {
public:
    Pid() = default;

    std::string_view to_regex() { return R"#(([0-9]+))#"; }
    Pid clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class Source {
public:
    Source() = default;

    std::string_view to_regex() { return R"#((.*?))#"; }
    Source clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class SourceFuncName {
public:
    SourceFuncName() = default;

    std::string_view to_regex() { return R"#((.*?))#"; }
    SourceFuncName clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class SourceLine {
public:
    SourceLine() = default;

    std::string_view to_regex() { return R"#(([0-9]+))#"; }
    SourceLine clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class Message {
public:
    Message() = default;

    std::string_view to_regex() { return R"#((.*?)$)#"; }
    Message clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class ScopeMessageBegin {
public:
    ScopeMessageBegin() = default;

    std::string_view to_regex() { return ""; }
    ScopeMessageBegin clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

class ScopeMessageEnd {
public:
    ScopeMessageEnd() = default;

    std::string_view to_regex() { return ""; }
    ScopeMessageEnd clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    const std::string& value() const { return value_;}
private:
    std::string value_;
};

using header_types_vec_t = std::vector<std::variant< Date
                                                   , Time
                                                   , LoggerName
                                                   , Level
                                                   , ThreadId
                                                   , Pid
                                                   , Source
                                                   , SourceFuncName
                                                   , SourceLine
                                                   , Message
                                                   >>;

// Pattern matching for function objects and creates a single function object 
// that can handle multiple callable objects.
template <typename... Ts> 
struct overload : Ts... { 

    // This line is creating an alias for the operator() member function from each of the 
    // base classes (types in Ts). This essentially means that the overload class 
    // has overloaded operator() functions from all the types provided as template arguments.
    using Ts::operator()...; 
}; 

/**
 * 
 */
class HeaderPattern {
public:
    HeaderPattern() = default;

    template <HeaderEntity... PatternArgs>
    explicit HeaderPattern(std::string_view pattern, PatternArgs... args) {

        if (int num_place_holders = count_place_holders(pattern); num_place_holders == sizeof...(PatternArgs)) {
            build_pattern(pattern, args...);
        } else {
            // TODO: throw
        }
    }

    // Receives a line and try to return the entities from it
    header_types_vec_t deserialize(const std::string& log_line) {
        header_types_vec_t results{};
        try {
            std::regex p(regex_);
            std::smatch match;
            std::deque<std::string> values;

            if (std::regex_search(log_line, match, p)) {
                for (auto i = 1; i < match.size(); ++i) {
                    values.push_back(match[i]);
                }
            }
            if (values.size() == type_sequence_.size()) {
                for (auto& type : type_sequence_) {
                    const auto value = values.front();
                    values.pop_front();

                    std::visit(overload {
                          [&value] (Date& date) { date.set_value(value); }
                        , [&value] (Time& time) { time.set_value(value); }
                        , [&value] (LoggerName& logger_name) { logger_name.set_value(value); }
                        , [&value] (Level& level) { level.set_value(value); }
                        , [&value] (ThreadId& thr_id) { thr_id.set_value(value); }
                        , [&value] (Pid& pid) { pid.set_value(value); }
                        , [&value] (Source& source) { source.set_value(value); }
                        , [&value] (SourceFuncName& func) { func.set_value(value); }
                        , [&value] (SourceLine& line) { line.set_value(value); }
                        , [&value] (Message& msg) { msg.set_value(value); }
                    }, type);
                }
            }
        } catch ([[maybe_unused]] std::regex_error &e) {
            header_types_vec_t{}.swap(results);
        }
        return results;
    }

    const std::string& regex_str() const { return regex_; }

private:
    template <typename... Entities>
    void build_pattern(std::string_view pattern, auto& entity, Entities... entities) { 
        if (pattern.empty()) { return; }

        while (!pattern.empty()) {
            if (pattern.starts_with(place_holder_)) {
                type_sequence_.push_back(std::move(entity.clone()));
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

    //Base case for the recursive function (no arguments left to process)
    void build_pattern([[maybe_unused]] std::string_view pattern) {
        if (pattern.empty()) { return; }

        regex_.append(pattern);
    }

    std::string escape_if_regex_symbol(char c) const {
        if (regex_metachars_.find_first_of(c) != std::string_view::npos) {
            std::string ret = fmt::format(R"(\{})", c);
            return ret;
        }
        return {c};
    }

    int count_place_holders(std::string_view pattern, int count = 0) const {
        if (pattern.empty()) { return count; }

        if (pattern.starts_with(place_holder_)) {
            return count_place_holders(pattern.substr(place_holder_.size()), count + 1);
        } else {
            return count_place_holders(pattern.substr(1), count);
        }
    }

    header_types_vec_t type_sequence_;
    constexpr static std::string_view regex_metachars_ = ".*+?|[]()^$\\";
    constexpr static std::string_view place_holder_ = "{}";
    std::string regex_;
};

} // namespace omlog::header