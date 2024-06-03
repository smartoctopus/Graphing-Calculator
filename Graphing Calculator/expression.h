#pragma once

enum class ExprKind {
    None,
    Add,
    Sub,
    Mul,
    Div,
    Pow,
    Neg,
    Sin,
    Cos,
    Tan,
    Num,
    X
};

struct Expr {
    ExprKind kind;

    union {
        struct {
            Expr* lhs;
            Expr* rhs;
        } binary;

        struct {
            Expr* parameter;
        } function;

        double number;
    };
};

Expr* expr_binary(ExprKind kind, Expr* lhs, Expr* rhs);
Expr* expr_function(ExprKind kind, Expr* parameter);
Expr* expr_number(double number);
Expr* expr_x();

double execute(Expr* expr, double x);

void dealloc_expr(Expr* expr);