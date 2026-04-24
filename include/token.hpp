#pragma once
#include <string>

namespace jc {
    enum class TokenType {
        IDENTIFIER,
        KEYWORD,
        NUMBER,
        STRING,
        OPERATOR,
        DELIMITER,
        END_OF_FILE,
        UNKNOWN
    };

    struct Token {
        TokenType type;
        std::string value;
        uint32_t line;
        uint32_t column;
    };
}