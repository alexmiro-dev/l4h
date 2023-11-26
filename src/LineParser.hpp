
#pragma once

#include "definitions.hpp"

#include <fmt/core.h>

#include <concepts>
#include <string_view>
#include <variant>
#include <vector>
#include <deque>
#include <regex>
#include <algorithm>
#include <utility>

// Reference: spdlog/pattern_formatter-inl.h

namespace l4h {


template <typename T>
concept LineEntities = requires(T entity, std::string_view value_sv) {
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

    [[nodiscard]] Date clone() const { return {*this}; }

    // TODO: convert this value to a tm structure 
    void set_value(std::string_view value) { value_ = value; }

    [[nodiscard]] const std::string& value() const { return value_;}

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
        : division_{time_division} {}

    std::string_view to_regex() {
        static constexpr auto hhmmss = R"#(\d{2}:\d{2}:\d{2})#";

        if (division_.unity != TimeDivision::Unity::None) {
            const std::string sep = division_.separator == '.' ? "\\." : std::string{division_.separator};
            pattern_ = fmt::format(R"#(({}{}\d{{{}}}))#", hhmmss, sep, division_.digits);
            return pattern_;
        }
        return hhmmss;
    }
    [[nodiscard]] Time clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    TimeDivision division_;
    std::string pattern_;
    std::string value_;
};

class LoggerName {
public:
    LoggerName() = default;
    std::string_view to_regex() { return R"#((.*?))#"; }
    [[nodiscard]] LoggerName clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

enum class LogLevel {Trace, Debug, Info, Warn, Err, Critical};

class Level {
public:
    Level() = default;
    static std::string_view to_regex() { return R"#((.*?))#"; }
    [[nodiscard]] Level clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

class ThreadId {
public:
    ThreadId() = default;

    // The thread ID could be represented by a hexadecimal value as well
    static std::string_view to_regex() { return R"#(([A-Fa-f0-9]+))#"; }
    [[nodiscard]] ThreadId clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

class Pid {
public:
    Pid() = default;

    static std::string_view to_regex() { return R"#(([0-9]+))#"; }
    [[nodiscard]] Pid clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

class Source {
public:
    Source() = default;

    static std::string_view to_regex() { return R"#((.*?))#"; }
    [[nodiscard]] Source clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

class SourceFuncName {
public:
    SourceFuncName() = default;

    static std::string_view to_regex() { return R"#((.*?))#"; }
    [[nodiscard]] SourceFuncName clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

class SourceLine {
public:
    SourceLine() = default;

    static std::string_view to_regex() { return R"#(([0-9]+))#"; }
    [[nodiscard]] SourceLine clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

class Message {
public:
    Message() = default;

    static std::string_view to_regex() { return R"#((.*?)$)#"; }
    [[nodiscard]] Message clone() const { return {*this}; }
    void set_value(std::string_view value) { value_ = value; }
    [[nodiscard]] const std::string& value() const { return value_;}
private:
    std::string value_;
};

using line_types_vec_t = std::vector<std::variant< Date
                                                 , Time
                                                 , LoggerName
                                                 , Level
                                                 , ThreadId
                                                 , Pid
                                                 , Source
                                                 , SourceFuncName
                                                 , SourceLine
                                                 , Message>>;

// Pattern matching for function objects and creates a single function object
// that can handle multiple callable objects.
template <typename... Ts>
struct Overload : Ts... {

    // This line is creating an alias for the operator() member function from each of the
    // base classes (types in Ts). This essentially means that the Overload class
    // has overloaded operator() functions from all the types provided as template arguments.
    using Ts::operator()...;
};

/**
 * 
 */
class LineParser {
public:
    LineParser() = default;

    template <LineEntities... PatternArgs>
    explicit LineParser(std::string_view pattern, PatternArgs... args) {

        if (int num_place_holders = count_place_holders(pattern); num_place_holders == sizeof...(PatternArgs)) {
            build_pattern(pattern, args...);
        } else {
            // TODO: throw
        }
    }

    // Receives a line and try to return the entities from it
    // Attention: this function was not designed to be thread safe, once it uses an internal attribute to hold the
    //            parsed values for the received line.
    line_types_vec_t deserialize(const std::string& log_line) {
        static std::regex p(regex_);
        static std::smatch match;
        static std::deque<std::string> values;
        static line_types_vec_t results = type_sequence_;
        static auto const type_sequence_size = type_sequence_.size();

        try {
            if (std::regex_search(log_line, match, p)) {
                for (auto i = 1; i < match.size(); ++i) {
                    values.push_back(match[i]);
                }
            }
            if (values.size() == type_sequence_size) {
                for (auto&& type : results) {
                    const auto value = values.front();
                    values.pop_front();

                    std::visit(Overload {
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
            line_types_vec_t{}.swap(results);
        }
        values.clear();
        return std::exchange(results, type_sequence_);
    }

    [[nodiscard]] const std::string& regex_str() const { return regex_; }

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
    void build_pattern(std::string_view pattern) {
        if (pattern.empty()) { return; }

        regex_.append(pattern);
    }

    [[nodiscard]] static std::string escape_if_regex_symbol(char c) {
        if (regex_metachars_.find_first_of(c) != std::string_view::npos) {
            std::string ret = fmt::format(R"(\{})", c);
            return ret;
        }
        return {c};
    }

    [[nodiscard]] int count_place_holders(std::string_view pattern, int count = 0) const {
        if (pattern.empty()) { return count; }

        if (pattern.starts_with(place_holder_)) {
            return count_place_holders(pattern.substr(place_holder_.size()), count + 1);
        } else {
            return count_place_holders(pattern.substr(1), count);
        }
    }

    line_types_vec_t type_sequence_;
    constexpr static std::string_view regex_metachars_ = ".*+?|[]()^$\\";
    constexpr static std::string_view place_holder_ = "{}";
    std::string regex_;
};

} // namespace l4h