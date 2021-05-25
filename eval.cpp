#include "eval.h"
#include "ast.h"
#include "object.h"
#include <iostream>
#include <stdexcept>

// TODO: start using unique pointers for more safety

namespace object_constant {
Null *null = new Null();
Boolean *TRUE_OBJ = new Boolean(true);
Boolean *FALSE_OBJ = new Boolean(false);
} // namespace object_constant

bool eval::is_error(Object *obj) {
	if (obj != nullptr) {
		return obj->Type() == objecttypes::ERROR;
	}
	return true;
}

Object *eval::len(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::STRING) {
		return new Error("len function is not supported for type: " +
										 objs[0]->Type());
	}

	int length = ((String *)objs[0])->value.size();
	return new Integer(length);
}

Object *eval::print(std::vector<Object *> &objs) {
	for (auto obj : objs) {
		if (eval::is_error(obj))
			return obj;
		std::cout << obj->Inspect() << " ";
	}
	std::cout << std::endl;

	return object_constant::null;
}

Object *eval::array_first(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto elements = ((Array *)objs[0])->elements;
	if (elements.size() > 0) {
		return elements[0];
	}

	return object_constant::null;
}

Object *eval::array_last(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto elements = ((Array *)objs[0])->elements;
	auto size = elements.size();
	if (size > 0) {
		return elements[size - 1];
	}

	return object_constant::null;
}

Object *eval::array_tail(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto elements = ((Array *)objs[0])->elements;
	auto size = elements.size();
	if (size > 0) {
		std::vector<Object *> tailed_elements(elements.begin() + 1, elements.end());
		return new Array(tailed_elements);
	}

	return object_constant::null;
}

Object *eval::array_push(std::vector<Object *> &objs) {
	if (objs.size() != 2) {
		return new Error("wrong number of arguments. want 2");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto array = (Array *)objs[0];
	auto length = array->elements.size();
	std::vector<Object *> new_elements = array->elements;
	new_elements.push_back(objs[1]);

	return new Array(new_elements);
}

std::unordered_map<std::string, Builtin *> builtin_functions = {
		{"len", new Builtin(*eval::len)},
		{"print", new Builtin(*eval::print)},
		{"last", new Builtin(*eval::array_last)},
		{"first", new Builtin(*eval::array_first)},
		{"tail", new Builtin(*eval::array_tail)},
		{"push", new Builtin(*eval::array_push)},
};

Object *eval::Eval(Node *node, Environment *env) {
	auto type = node->Type();
	if (type == "IntegerLiteral") {
		Integer *obj = new Integer(((IntegerLiteral *)node)->value);
		return obj;
	} else if (type == "Program") {
		return eval::eval_statements(node, env);
	} else if (type == "ExpressionStatement") {
		auto exps = (ExpressionStatement *)node;
		return eval::Eval(exps->expression.get(), env);
	} else if (type == "BooleanExpression") {
		bool truthy = ((BooleanExpression *)node)->value;
		if (truthy) {
			return object_constant::TRUE_OBJ;
		}
		return object_constant::FALSE_OBJ;
	} else if (type == "PrefixExpression") {
		auto pre = (PrefixExpression *)node;
		auto right = Eval(pre->right.get(), env);
		return eval::eval_prefix_expression(pre->opr, right);
	} else if (type == "InfixExpression") {
		auto inf = (InfixExpression *)node;
		auto right = eval::Eval(inf->right.get(), env);
		auto left = eval::Eval(inf->left.get(), env);

		return eval::eval_infix_exp(inf->opr, right, left);
	} else if (type == "BlockExpression") {
		return eval::eval_blockstatement(node, env);
	} else if (type == "IfExpression") {
		auto ifexp = (IfExpression *)node;
		return eval::eval_if_expression(ifexp, env);
	} else if (type == "LetStatement") {
		auto value = eval::Eval(((LetStatement *)node)->value.get(), env);
		if (eval::is_error(value))
			return value;
		env->set(((LetStatement *)node)->name->value, value);
	} else if (type == "ReturnStatement") {
		auto val = eval::Eval(((ReturnStatement *)node)->return_value.get(), env);
		return new Return(val);
	} else if (type == "Identifier") {
		return eval::eval_identifier((Identifier *)node, env);
	} else if (type == "FunctionLiteral") {
		return eval::eval_function_literal(node, env);
	} else if (type == "CallExpression") {
		return eval::eval_call_expression(node, env);
	} else if (type == "StringLiteral") {
		// for some reason the value doesn't work but the function literal works
		return new String(((StringLiteral *)node)->TokenLiteral());
	} else if (type == "ArrayLiteral") {
		return eval::eval_array_literal(node, env);
	} else if (type == "IndexExpression") {
		auto array = eval::Eval(((IndexExpression *)node)->left.get(), env);
		if (eval::is_error(array)) {
			return array;
		}
		auto index = eval::Eval(((IndexExpression *)node)->index.get(), env);
		if (eval::is_error(index)) {
			return index;
		}

		return eval_index_expression(array, index);
	}

	return nullptr;
}

Object *eval::eval_statements(Node *program, Environment *env) {
	Object *res;
	auto pg = dynamic_cast<Program *>(program);

	for (auto &st : pg->statements) {
		res = eval::Eval(st.get(), env);

		if (res != nullptr) {
			if (res->Type() == objecttypes::RETURN)
				return ((Return *)res)->value;
			else if (res->Type() == objecttypes::ERROR)
				return ((Error *)res);
		}
	}

	return res;
}

Object *eval::eval_prefix_expression(std::string opr, Object *right) {
	if (opr == "!") {
		return eval::eval_bang_exp(right);
	} else if (opr == "-") {
		return eval::eval_minus_exp(right);
	}

	return new Error("unknown operation: " + opr + right->Type());
}

Object *eval::eval_bang_exp(Object *right) {
	if (right == object_constant::TRUE_OBJ) {
		return object_constant::FALSE_OBJ;
	} else if (right == object_constant::FALSE_OBJ) {
		return object_constant::TRUE_OBJ;
	} else if (right == object_constant::null) {
		return object_constant::TRUE_OBJ;
	}

	return object_constant::FALSE_OBJ;
}

Object *eval::eval_minus_exp(Object *right) {
	if (right->Type() == objecttypes::INTEGER) {
		Integer *obj = new Integer(-((Integer *)right)->value);
		return obj;
	}

	return new Error("unknown operation: -" + right->Type());
}

Object *eval::eval_infix_exp(std::string opr, Object *right, Object *left) {
	if (left->Type() == objecttypes::INTEGER &&
			right->Type() == objecttypes::INTEGER)
		return eval::eval_integer_infix(opr, right, left);
	else if (left->Type() != right->Type()) {
		return new Error("wrong types: " + left->Type() + " " + opr + " " +
										 right->Type());
	} else if (left->Type() == objecttypes::STRING &&
						 right->Type() == objecttypes::STRING) {
		return eval::eval_string_infix(opr, right, left);
	} else if (opr == "==") {
		return eval::boolean_to_object(left == right);
	} else if (opr == "!=") {
		return eval::boolean_to_object(left != right);
	}

	return new Error("unknown operation: " + left->Type() + " " + opr + " " +
									 right->Type());
}

Object *eval::eval_integer_infix(std::string opr, Object *right, Object *left) {
	auto left_val = ((Integer *)left)->value;
	auto right_val = ((Integer *)right)->value;

	if (opr == "+") {
		return new Integer(left_val + right_val);
	} else if (opr == "-") {
		return new Integer(left_val - right_val);
	} else if (opr == "*") {
		return new Integer(left_val * right_val);
	} else if (opr == "/") {
		return new Integer(left_val / right_val);
	} else if (opr == "<")
		return eval::boolean_to_object(left_val < right_val);
	else if (opr == ">")
		return eval::boolean_to_object(left_val > right_val);
	else if (opr == "==")
		return eval::boolean_to_object(left_val == right_val);
	else if (opr == "!=")
		return eval::boolean_to_object(left_val != right_val);

	return new Error("unknown operation: " + left->Type() + " " + opr + " " +
									 right->Type());
}

Object *eval::boolean_to_object(bool value) {
	return (value ? object_constant::TRUE_OBJ : object_constant::FALSE_OBJ);
}

bool eval::is_true(Object *obj) {
	if (obj == object_constant::null) {
		return false;
	} else if (obj == object_constant::TRUE_OBJ) {
		return true;
	} else if (obj == object_constant::FALSE_OBJ) {
		return false;
	}

	return true;
}

Object *eval::eval_if_expression(IfExpression *ifexp, Environment *env) {
	auto cond = eval::Eval(ifexp->cond.get(), env);

	if (eval::is_true(cond)) {
		return eval::Eval(ifexp->after.get(), env);
	} else if (ifexp->other != nullptr) {
		return eval::Eval(ifexp->other.get(), env);
	}

	return object_constant::null;
}

Object *eval::eval_blockstatement(Node *blockexp, Environment *env) {
	Object *res;
	auto bckexp = dynamic_cast<BlockStatement *>(blockexp);

	for (auto &st : bckexp->statements) {
		res = eval::Eval(st.get(), env);

		if (res != nullptr) {
			auto tp = res->Type();
			if (tp == objecttypes::RETURN || tp == objecttypes::ERROR)
				return res;
		}
	}

	return res;
}

Object *eval::eval_identifier(Node *ident, Environment *env) {
	auto id = dynamic_cast<Identifier *>(ident);

	auto value = env->get(id->value);
	if (value->Type() == objecttypes::ERROR &&
			builtin_functions.find(id->value) != builtin_functions.end()) {
		return builtin_functions[id->value];
	}

	return value;
}

Object *eval::eval_function_literal(Node *node, Environment *env) {
	auto fn = dynamic_cast<FunctionLiteral *>(node);
	std::vector<Identifier *> params;
	params.reserve(fn->params.size());

	for (const auto &ident : fn->params) {
		params.push_back(ident.get());
	}

	auto obj = new Function();
	obj->params = params;
	obj->env = env;
	obj->body = fn->body.get();

	return obj;
}

std::vector<Object *> eval::eval_expressions(std::vector<Expression *> &exps,
																						 Environment *env) {
	auto res = std::vector<Object *>();

	for (const auto &exp : exps) {
		auto evaluated = eval::Eval(exp, env);
		if (evaluated != nullptr && evaluated->Type() == objecttypes::ERROR) {
			auto other = std::vector<Object *>();
			other.reserve(1);
			other.push_back(evaluated);
			return other;
		}

		res.push_back(evaluated);
	}

	return res;
}

Object *eval::eval_call_expression(Node *node, Environment *env) {
	auto callexp = dynamic_cast<CallExpression *>(node);
	auto func = eval::Eval(callexp->func.get(), env);
	if (func != nullptr && func->Type() == objecttypes::ERROR)
		return func;
	auto args = std::vector<Expression *>();
	args.reserve(callexp->arguments.size());
	for (const auto &exp : callexp->arguments)
		args.push_back(exp.get());

	auto eval_args = eval::eval_expressions(args, env);
	// an error has occured with evaluating the arguments
	if (eval_args.size() == 1 && eval_args[0]->Type() == objecttypes::ERROR)
		return eval_args[0];
	return eval::apply_function(func, eval_args);
}

Object *eval::apply_function(Object *func, std::vector<Object *> &args) {
	if (func->Type() == objecttypes::BUILTIN)
		return ((Builtin *)func)->func(args);

	if (func->Type() != objecttypes::FUNCTION)
		return new Error("not a function, got: " + func->Type());

	auto extended = eval::extend_function_env(func, args);
	auto evaluated = eval::Eval(((Function *)func)->body, extended);

	return eval::unwrap_return(evaluated);
}

Environment *eval::extend_function_env(Object *func,
																			 std::vector<Object *> &args) {
	auto fn = dynamic_cast<Function *>(func);
	auto env = new Environment(fn->env);

	for (int i = 0; i < fn->params.size(); ++i)
		env->set(fn->params[i]->value, args[i]);

	return env;
}

Object *eval::unwrap_return(Object *obj) {
	if (obj->Type() == objecttypes::RETURN)
		return (((Return *)obj)->value);
	return obj;
}

Object *eval::eval_string_infix(const std::string &opr, Object *right,
																Object *left) {
	if (opr != "+") {
		return new Error("unknown operation: " + left->Type() + " " + opr + " " +
										 right->Type());
	}

	auto left_val = ((String *)left)->value;
	auto right_val = ((String *)right)->value;

	return new String(left_val + right_val);
}

Object *eval::eval_array_literal(Node *node, Environment *env) {
	auto arrlit = dynamic_cast<ArrayLiteral *>(node);

	std::vector<Expression *> elements;
	for (const auto &elem : arrlit->elements)
		elements.push_back(elem.get());

	auto evaluated_elems = eval::eval_expressions(elements, env);
	return new Array(evaluated_elems);
}

Object *eval::eval_index_expression(Object *left, Object *index) {
	if (left->Type() == objecttypes::ARRAY_OBJ &&
			index->Type() == objecttypes::INTEGER) {
		return eval::eval_array_index_expression((Array *)left, (Integer *)index);
	}

	return new Error("index operation not found: " + left->Type());
}

Object *eval::eval_array_index_expression(Array *arr, Integer *index) {
	auto idx = index->value;
	auto max = arr->elements.size() - 1;

	if (idx < 0 || idx > max) {
		return object_constant::null;
	}

	return arr->elements[idx];
}
