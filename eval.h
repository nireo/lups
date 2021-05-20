#ifndef LUPS_EVAL_H
#define LUPS_EVAL_H

#include "ast.h"
#include "object.h"

namespace eval {
Object *Eval(Node *node);
Object *eval_statements(Node *program);
Object *eval_prefix_expression(const std::string &opr, Object *right);
Object *eval_bang_exp(Object *right);
} // namespace eval

#endif
