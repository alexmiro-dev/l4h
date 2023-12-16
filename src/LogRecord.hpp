
#pragma once

#include "definitions.hpp"
#include "LineParser.hpp"

namespace l4h {

/**
 * Holds the semantic information of a log line after being parsed
 */
class LogRecord {
public:

    void set_type(defs::LogRecordType type) { type_ = type; }
    [[nodiscard]] defs::LogRecordType type() const { return type_; }

    void set_uid(defs::line_uid_t uid) { uid_ = uid; }
    [[nodiscard]] defs::line_uid_t uid() const { return uid_; }

    void set_parent_uid(defs::line_uid_t uid) { parent_uid_ = uid; }
    [[nodiscard]] defs::line_uid_t parent_uid() const { return parent_uid_; }

    void set_tokens(line_types_vec_t tokens) { tokens_ = tokens; }

    template <defs::LineEntity RequestedEntity>
    std::optional<RequestedEntity> get() const {
        if (std::is_same_v<Message, RequestedEntity> && type_ == defs::LogRecordType::MessagePart) {
            if (message_.value().empty()) {
                return std::nullopt;
            }
            return message_;
        }
        if (!tokens_) {
            return std::nullopt;
        }
        RequestedEntity result;

        for (auto&& token : tokens_.value()) {
            std::visit([&result] (auto&& entity) {
                using EntityType = std::decay_t<decltype(entity)>;

                if constexpr (std::is_same_v<EntityType, RequestedEntity>) {
                    result = entity.clone();
                }
            }, token);

            if (!result.value().empty()) {
                return result;
            }
        }
        return std::nullopt;
    }

    void set_message_part(std::string const& part) { message_.set_value(part); }

private:
    std::optional<line_types_vec_t> tokens_;
    defs::LogRecordType type_{defs::LogRecordType::Unknown};
    defs::line_uid_t uid_{defs::g_line_no_uid};
    defs::line_uid_t parent_uid_{defs::g_line_no_uid};
    Message message_;
};

} // namespace l4h
