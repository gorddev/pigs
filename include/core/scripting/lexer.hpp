#pragma once
#include <string_view>
#include <cctype>

namespace pg::script {

    enum class TokenType { Identifier, Assignment, Number, Boolean, EndOfFile, Invalid };

    struct Token {
        TokenType type;
        std::string_view value;
    };

    class Lexer {
    private:
        std::string_view source;
        size_t cursor = 0;

        void skipWhitespace() {
            while (cursor < source.size() && std::isspace(source[cursor])) {
                cursor++;
            }
        }

    public:
        explicit Lexer(std::string_view src) : source(src) {}

        Token nextToken() {
            skipWhitespace();
            if (cursor >= source.size()) return { TokenType::EndOfFile, "" };

            size_t start = cursor;
            char c = source[cursor];

            if (std::isalpha(c) || c == '_') {
                while (cursor < source.size() && (std::isalnum(source[cursor]) || source[cursor] == '_')) {
                    cursor++;
                }
                std::string_view val = source.substr(start, cursor - start);
                if (val == "true" || val == "false") return { TokenType::Boolean, val };
                return { TokenType::Identifier, val };
            }

            if (std::isdigit(c)) {
                while (cursor < source.size() && std::isdigit(source[cursor])) {
                    cursor++;
                }
                return { TokenType::Number, source.substr(start, cursor - start) };
            }

            if (c == '=') {
                cursor++;
                return { TokenType::Assignment, source.substr(start, 1) };
            }

            cursor++;
            return { TokenType::Invalid, source.substr(start, 1) };
        }
    };
}
