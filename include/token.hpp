#pragma once
#include <string>
#include <format>
#include "types.hpp"
#include "utils.hpp"

namespace jc
{
    enum class TokenType : u8
    {
        IDENTIFIER,
        KEYWORD,
        NUMBER,
        OPERATOR,
        DELIMITER,
        END_OF_FILE,
        UNKNOWN
    };

    struct Token
    {
        TokenType type;
        std::string value;
        u32 line;
        u32 column;

        std::string to_string() const
        {
            return std::format("({}, {})", value, jc::to_string(type));
        }
    };
}