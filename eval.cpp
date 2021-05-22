#include "eval.h"
#include "ast.h"
#include "object.h"
#include <stdexcept>

// TODO: start using unique pointers for more safety

namespace object_constant {
Null *null = new Null();
Boolean *TRUE_OBJ = new Boolean(true);
Boolean *FALSE_OBJ = new Boolean(false);
} // namespace object_constant

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
		if (value->Type() == objecttypes::ERROR)
			return value;
		env->set(((LetStatement *)node)->name->value, value);
	} else if (type == "ReturnStatement") {
		auto val = eval::Eval(((ReturnStatement *)node)->return_value.get(), env);
		return new Return(val);
	} else if (type == "Identifier") {
		return eval::eval_identifier((Identifier *)node, env);
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
	if (value == nullptr) {
		return new Error("identifier not found: " + id->value);
	}

	return value;
}
