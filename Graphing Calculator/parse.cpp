#include <iostream>
#include <charconv>

#include "parse.h"

void Parser::tokenize()
{
    while (m_cursor < m_string.length())
        m_tokens.push_back(next_token());

    // Reset the cursor, beacuse it will be used to index the m_tokens vector
    m_cursor = 0;
}

#define OP1(_CH, _KIND)                                                 \
    case _CH: {                                                         \
        advance();                                                      \
        return Token(TokenKind::##_KIND, std::string_view(here() - 1, 1));   \
    }

Parser::Token Parser::next_token()
{
    while (isspace(current()))
        advance();

    if (isalpha(current())) {
        const char* start = here();
        size_t length = 0;

        while (isalpha(current())) {
            advance();
            length++;
        }

        std::string_view identifier(start, length);

        return Token(TokenKind::Identifier, identifier);
    }

    if (isdigit(current())) {
        const char* start = here();
        size_t length = 0;

        while (isdigit(current())) {
            advance();
            length++;
        }

        if (current() == '.') {
            advance();
            length++;

            while (isdigit(current())) {
                advance();
                length++;
            }
        }

        std::string_view number(start, length);

        return Token(TokenKind::Number, number);
    }

    switch (current()) {
        OP1('+', Plus);
        OP1('-', Minus);
        OP1('*', Star);
        OP1('/', Slash);
        OP1('^', Caret);
        OP1('(', LParen);
        OP1(')', RParen);
    }

    std::cerr << "[ERROR] Invalid character '" << current() << "'" << std::endl;
    advance();

    return Token(TokenKind::Error, std::string_view(here() - 1, 1));
}

bool Parser::consume(TokenKind kind, std::string_view error_string)
{
    if (token().kind != kind) {
        std::cerr << "[ERROR] " << error_string << std::endl;
        return false;
    }

    advance();
    return true;
}

Parser::Precedence Parser::get_precedence(Token token) {
    switch (token.kind) {
    case TokenKind::Plus:
        return PREC_TERM;
    case TokenKind::Minus:
        return PREC_TERM;
    case TokenKind::Slash:
        return PREC_FACTOR;
    case TokenKind::Star:
        return PREC_FACTOR;
    case TokenKind::Caret:
        return PREC_POW;
    case TokenKind::RParen:
        return PREC_NONE;
    default:
        advance();
        return PREC_NONE;
    }
}

ExprKind Parser::expr_kind(Token token) {
    switch (token.kind) {
    case TokenKind::Plus:
        return ExprKind::Add;
    case TokenKind::Minus:
        return ExprKind::Sub;
    case TokenKind::Star:
        return ExprKind::Mul;
    case TokenKind::Slash:
        return ExprKind::Div;
    case TokenKind::Caret:
        return ExprKind::Pow;
    case TokenKind::Number:
        return ExprKind::Num;
    case TokenKind::Identifier: {
        if (token.str == "sin")
            return ExprKind::Sin;
        else if (token.str == "cos")
            return ExprKind::Cos;
        else if (token.str == "tan")
            return ExprKind::Tan;
        else if (token.str == "x")
            return ExprKind::X;
    } /* fall-through */
    default: {
        std::cerr << "[ERROR] Invalid token" << std::endl;
        advance();
        return ExprKind::None;
    }
    }
}

Expr* Parser::parse_lhs() {
    switch (token().kind) {
    case TokenKind::Minus: {
        advance();
        return expr_function(ExprKind::Neg, parse_expression(PREC_UNARY));
    } break;
    case TokenKind::Number: {
        double number;
        std::from_chars(token().str.data(), token().str.data() + token().str.size(), number);
        advance();
        return expr_number(number);
    } break;
    case TokenKind::LParen: {
        advance();
        Expr* expr = parse_expression(PREC_TERM);
        consume(TokenKind::RParen, "Expected closing parenthesis");
        return expr;
    } break;
    case TokenKind::Identifier: {
        if (token().str == "x") {
            advance();
            return expr_x();
        }
        else if (token().str == "sin" || token().str == "cos" || token().str == "tan") {
            ExprKind kind = expr_kind(token());
            advance();

            consume(TokenKind::LParen, "Expected opening parenthesis");
            Expr* parameter = parse_expression(PREC_TERM);
            consume(TokenKind::RParen, "Expected closing parenthesis");

            return expr_function(kind, parameter);
        }
    } break;
    }

    std::cerr << "[ERROR] Invalid token" << std::endl;
    advance();
    return nullptr;
}

Expr* Parser::parse_expression(int prec) {
    Expr* lhs = parse_lhs();
    
    while(prec <= get_precedence(token())) {
        Precedence current_prec = get_precedence(token());
        ExprKind kind = expr_kind(token());
        advance();

        lhs = expr_binary(kind, lhs, parse_expression(current_prec + 1));
    }

    return lhs;
}

Expr* Parser::parse()
{
    tokenize();

    return parse_expression(PREC_TERM);
}