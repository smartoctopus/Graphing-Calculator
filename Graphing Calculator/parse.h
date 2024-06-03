#pragma once

#include <string>
#include <vector>
#include <string_view>

#include "expression.h"

class Parser {
public:
    Parser(std::string str) : m_string(str) {}

    Expr* parse();
private:
    // Common (Lexing and Parsing)
    inline size_t cursor() const {
        return m_cursor;
    }

    inline void advance() {
        m_cursor++;
    }

    // Lexing
    inline const char* here() const {
        return m_string.c_str() + m_cursor;
    }

    inline char current() const {
        return m_string[m_cursor];
    }

    enum class TokenKind {
        None,
        Error,
        Plus,
        Minus,
        Star,
        Slash,
        Caret,
        LParen,
        RParen,
        Identifier,
        Number
    };

    struct Token {
        Token(TokenKind kind, std::string_view str) : kind(kind), str(str) {}

        TokenKind kind;
        std::string_view str;
    };

    Token next_token();
    void tokenize();

    // Parsing
    inline Token token() const {
        if (m_cursor < m_tokens.size()) {
            return m_tokens[m_cursor];
        }

        return Token(TokenKind::None, "");
    }

    bool consume(TokenKind kind, std::string_view error_string);

    enum Precedence {
        PREC_NONE,
        PREC_TERM,
        PREC_FACTOR,
        PREC_POW,
        PREC_UNARY,
        PREC_PRIMARY
    };

    Precedence get_precedence(Token token);

    ExprKind expr_kind(Token token);

    Expr* parse_lhs();
    Expr* parse_expression(int prec);

    size_t m_cursor = 0;
    std::string m_string;
    std::vector<Token> m_tokens;
};