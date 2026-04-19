#pragma once
#include <string>
#include "ast.h"

// ─── AST Pretty Printer ───────────────────────────────────────────────────────
// Walks the AST and prints a human-readable tree to stdout.
// Used by the --debug flag in the pipeline.

class ASTPrinter {
private:
    int indent = 0;

    void pad() const;
    std::string opName(Tokentype op) const;

    void printExpr(const Expr* expr);
    void printStmt(const Stmt* stmt);

public:
    void print(const std::vector<std::unique_ptr<Stmt>>& stmts);
};
