#include <cmath>
#include <iostream>
#include <cassert>

#include "expression.h"
#include "utility.h"

Expr* expr_binary(ExprKind kind, Expr* lhs, Expr* rhs) {
    assert(kind == ExprKind::Add || kind == ExprKind::Sub || kind == ExprKind::Mul || kind == ExprKind::Div || kind == ExprKind::Pow);

    Expr* expr = xmalloc<Expr>();

    expr->kind = kind;
    expr->binary.lhs = lhs;
    expr->binary.rhs = rhs;

    return expr;
}

Expr* expr_function(ExprKind kind, Expr* parameter) {
    assert(kind == ExprKind::Sin || kind == ExprKind::Cos || kind == ExprKind::Tan || kind == ExprKind::Neg);

    Expr* expr = xmalloc<Expr>();

    expr->kind = kind;
    expr->function.parameter = parameter;

    return expr;
}

Expr* expr_number(double number) {
    Expr* expr = xmalloc<Expr>();

    expr->kind = ExprKind::Num;
    expr->number = number;

    return expr;
}

Expr* expr_x() {
    Expr* expr = xmalloc<Expr>();

    expr->kind = ExprKind::X;

    return expr;
}

#define BINARY(_TYPE, _OP)                         \
    case ExprKind::##_TYPE: {                      \
        double lhs = execute(expr->binary.lhs, x); \
        double rhs = execute(expr->binary.rhs, x); \
        return lhs _OP rhs;                        \
    }

#define FUNCTION(_TYPE, _FN)                              \
    case ExprKind::##_TYPE: {                             \
        return _FN(execute(expr->function.parameter, x)); \
    }

double execute(Expr* expr, double x)
{
    if (expr == nullptr)
        return NAN;

    switch (expr->kind) {
        BINARY(Add, +);
        BINARY(Sub, -);
        BINARY(Mul, *);
        BINARY(Div, / );

        FUNCTION(Sin, sin);
        FUNCTION(Cos, cos);
        FUNCTION(Tan, tan);

    case ExprKind::Neg: {
        double num = execute(expr->function.parameter, x);
        return -num;
    }

    case ExprKind::Pow: {
        double lhs = execute(expr->binary.lhs, x);
        double rhs = execute(expr->binary.rhs, x);
        return pow(lhs, rhs);
    }

    case ExprKind::Num: {
        return expr->number;
    }

    case ExprKind::X: {
        return x;
    }

    case ExprKind::None:
    default: {
        std::cerr << "Invalid Expression!" << std::endl;
    }
    }
}

void dealloc_expr(Expr* expr)
{
    if (expr == nullptr)
        return;

    switch (expr->kind) {
    case ExprKind::Add:
    case ExprKind::Sub:
    case ExprKind::Mul:
    case ExprKind::Pow:
    case ExprKind::Div: {
        dealloc_expr(expr->binary.lhs);
        dealloc_expr(expr->binary.rhs);
    } break;

    case ExprKind::Sin:
    case ExprKind::Cos:
    case ExprKind::Tan: {
        dealloc_expr(expr->function.parameter);
    } break;
    
    default: break; // Skip everything else
    }

    free(expr);
}