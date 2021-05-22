#ifndef LUPS_EVAL_H
#define LUPS_EVAL_H

#include "ast.h"
#include "object.h"

namespace object_constant {
extern Null *null;
extern Boolean *TRUE_OBJ;
extern Boolean *FALSE_OBJ;
} // namespace object_constant

namespace eval {
Object *Eval(Node *node);
Object *eval_statements(Node *program);
Object *eval_prefix_expression(std::string opr, Object *right);
Object *eval_bang_exp(Object *right);
Object *eval_minus_exp(Object *right);
Object *eval_infix_exp(std::string opr, Object *right, Object *left);
Object *eval_integer_infix(std::string opr, Object *right, Object *left);
Object *eval_boolean_infix(std::string opr, Object *right, Object *left);
Object *eval_blockstatement(Node *blockexp);
Object *boolean_to_object(bool value);
bool is_true(Object *obj);
Object *eval_if_expression(IfExpression *ifexp);
} // namespace eval

#endif
