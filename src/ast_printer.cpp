#include "ast_printer.h"
#include <iostream>

void ASTPrinter::pad() const {
    for (int i = 0; i < indent; i++) std::cout << "  ";
}

std::string ASTPrinter::opName(Tokentype op) const {
    switch (op) {
        case Tokentype::PLUS:          return "+";
        case Tokentype::MINUS:         return "-";
        case Tokentype::STAR:          return "*";
        case Tokentype::SLASH:         return "/";
        case Tokentype::EQUAL_EQUAL:   return "==";
        case Tokentype::BANG_EQUAL:    return "!=";
        case Tokentype::LESS:          return "<";
        case Tokentype::LESS_EQUAL:    return "<=";
        case Tokentype::GREATER:       return ">";
        case Tokentype::GREATER_EQUAL: return ">=";
        default:                       return "?";
    }
}

void ASTPrinter::printExpr(const Expr* expr) {
    if (auto* n = dynamic_cast<const NumberExpr*>(expr)) {
        std::cout << "NumberExpr(" << n->val << ")";

    } else if (auto* b = dynamic_cast<const BoolExpr*>(expr)) {
        std::cout << "BoolExpr(" << (b->val ? "true" : "false") << ")";

    } else if (auto* v = dynamic_cast<const VarExpr*>(expr)) {
        std::cout << "VarExpr(\"" << v->name << "\")";

    } else if (auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
        std::cout << "BinaryExpr(" << opName(bin->op) << ")\n";
        indent++;
        pad(); std::cout << "|-- "; printExpr(bin->left.get());  std::cout << "\n";
        pad(); std::cout << "\\-- "; printExpr(bin->right.get()); std::cout << "\n";
        indent--;

    } else if (auto* un = dynamic_cast<const UnaryExpr*>(expr)) {
        std::cout << "UnaryExpr(" << opName(un->op) << ")\n";
        indent++;
        pad(); std::cout << "\\-- "; printExpr(un->right.get()); std::cout << "\n";
        indent--;
    }
}

void ASTPrinter::printStmt(const Stmt* stmt) {
    if (auto* s = dynamic_cast<const LetStmt*>(stmt)) {
        pad(); std::cout << "LetStmt(\"" << s->name << "\")\n";
        indent++;
        pad(); std::cout << "\\-- init: "; printExpr(s->init.get()); std::cout << "\n";
        indent--;

    } else if (auto* s = dynamic_cast<const AssignStmt*>(stmt)) {
        pad(); std::cout << "AssignStmt(\"" << s->name << "\")\n";
        indent++;
        pad(); std::cout << "\\-- value: "; printExpr(s->value.get()); std::cout << "\n";
        indent--;

    } else if (auto* s = dynamic_cast<const PrintStmt*>(stmt)) {
        pad(); std::cout << "PrintStmt\n";
        indent++;
        pad(); std::cout << "\\-- "; printExpr(s->value.get()); std::cout << "\n";
        indent--;

    } else if (auto* s = dynamic_cast<const InputStmt*>(stmt)) {
        pad(); std::cout << "InputStmt(\"" << s->name << "\")\n";

    } else if (auto* s = dynamic_cast<const IfStmt*>(stmt)) {
        pad(); std::cout << "IfStmt\n";
        indent++;
        pad(); std::cout << "|-- condition: "; printExpr(s->condition.get()); std::cout << "\n";
        pad(); std::cout << "|-- then:\n";
        indent++;
        for (auto& st : s->thenBranch) printStmt(st.get());
        indent--;
        if (!s->elseBranch.empty()) {
            pad(); std::cout << "\\-- else:\n";
            indent++;
            for (auto& st : s->elseBranch) printStmt(st.get());
            indent--;
        }
        indent--;

    } else if (auto* s = dynamic_cast<const WhileStmt*>(stmt)) {
        pad(); std::cout << "WhileStmt\n";
        indent++;
        pad(); std::cout << "|-- condition: "; printExpr(s->condition.get()); std::cout << "\n";
        pad(); std::cout << "\\-- body:\n";
        indent++;
        for (auto& st : s->body) printStmt(st.get());
        indent--;
        indent--;
    }
}

void ASTPrinter::print(const std::vector<std::unique_ptr<Stmt>>& stmts) {
    std::cout << "\n=======================================\n";
    std::cout << "          ABSTRACT SYNTAX TREE         \n";
    std::cout << "=======================================\n";
    for (auto& stmt : stmts) {
        printStmt(stmt.get());
    }
    std::cout << "\n";
}