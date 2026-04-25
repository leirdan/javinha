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

    inline std::string to_string(TokenType t) {
        switch (t) {
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::KEYWORD: return "KEYWORD";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::STRING: return "STRING";
            case TokenType::OPERATOR: return "OPERATOR";
            case TokenType::DELIMITER: return "DELIMITER";
            case TokenType::END_OF_FILE: return "EOF";
            default: return "UNKNOWN";
        }
    }
    struct Token {
        TokenType type;
        std::string value;
        uint32_t line;
        uint32_t column;
    };
}