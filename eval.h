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
Object *Eval(Node *node, Environment *env);
Object *eval_statements(Node *program, Environment *env);
Object *eval_prefix_expression(std::string opr, Object *right);
Object *eval_bang_exp(Object *right);
Object *eval_minus_exp(Object *right);
Object *eval_infix_exp(std::string opr, Object *right, Object *left);
Object *eval_integer_infix(std::string opr, Object *right, Object *left);
Object *eval_boolean_infix(std::string opr, Object *right, Object *left);
Object *eval_blockstatement(Node *blockexp, Environment *env);
Object *boolean_to_object(bool value);
Object *eval_if_expression(IfExpression *ifexp, Environment *env);
Object *eval_identifier(Node *ident, Environment *env);
Object *eval_function_literal(Node *func, Environment *env);
bool is_true(Object *obj);
std::vector<Object *> eval_expressions(std::vector<Expression *> &exps,
																			 Environment *env);
Object *eval_call_expression(Node *node, Environment *env);
Object *apply_function(Object *func, std::vector<Object *> &args);
Environment *extend_function_env(Object *func, std::vector<Object *> &args);
Object *unwrap_return(Object *obj);
Object *eval_string_infix(const std::string &opr, Object *right, Object *left);
Object *len(std::vector<Object *> &objs);
Object *eval_array_literal(Node *node, Environment *env);
Object *eval_index_expression(Object *left, Object *index, Environment *env);
Object *eval_array_index_expression(Array *arr, Integer *index);
bool is_error(Object *obj);
Object *print(std::vector<Object *> &objs);
Object *array_first(std::vector<Object *> &objs);
Object *array_tail(std::vector<Object *> &objs);
Object *array_last(std::vector<Object *> &objs);
Object *array_push(std::vector<Object *> &objs);
Object *eval_hash_literal(Node *node, Environment *env);
Object *eval_hash_index_expression(Object *left, Object *index,
																	 Environment *env);
} // namespace eval

#endif
