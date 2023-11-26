
#pragma once

#include "definitions.hpp"

namespace l4h {

/**
 * Holds the semantic information of a log line after being parsed
 */
class LogRecord {
public:
    enum class Type { Invalid, HeaderAndMessage, MessagePart };

    void set_type(Type type) { type_ = type; }
    [[nodiscard]] Type type() const { return type_; }

    void set_uid(defs::line_uid_t uid) { uid_ = uid; }
    [[nodiscard]] defs::line_uid_t uid() const { return uid_; }

    void set_parent_uid(defs::line_uid_t uid) { parent_uid_ = uid; }
    [[nodiscard]] defs::line_uid_t parent_uid() const { return parent_uid_; }

    void set_tokens(line_types_vec_t tokens) { tokens_ = tokens; }

    template <typename RequestedEntity>
    std::optional<RequestedEntity> get() const {
        if (std::is_same_v<Message, RequestedEntity> && type_ == Type::MessagePart) {
            if (message_part_.value().empty()) {
                return std::nullopt;
            }
            return message_part_;
        }
        if (!tokens_) {
            return std::nullopt;
        }
        for (auto&& token : tokens_.value()) {
            std::visit([] (auto&& entity) {
                using EntityType = std::decay_t<decltype(entity)>;

                if constexpr (std::is_same_v<EntityType, RequestedEntity>) {
                    return entity.value();
                }
            }, token);
        }
        return std::nullopt;
    }

    void set_message_part(std::string const& part) { message_part_.set_value(part); }

private:
    std::optional<line_types_vec_t> tokens_;
    Type type_{Type::Invalid};
    defs::line_uid_t uid_{defs::g_line_no_uid};
    defs::line_uid_t parent_uid_{defs::g_line_no_uid};
    Message message_part_;
};

} // namespace l4h
