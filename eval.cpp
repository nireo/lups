#include "eval.h"
#include "ast.h"
#include "object.h"

// TODO: start using unique pointers

namespace object_constant {
Null *null = new Null();
Boolean *TRUE_OBJ = new Boolean(true);
Boolean *FALSE_OBJ = new Boolean(false);
} // namespace object_constant

Object *eval::Eval(Node *node) {
	auto type = node->Type();
	if (type == "IntegerLiteral") {
		Integer *obj = new Integer(((IntegerLiteral *)node)->value);
		return obj;
	} else if (type == "Program") {
		return eval::eval_statements(node);
	} else if (type == "ExpressionStatement") {
		auto exps = (ExpressionStatement *)node;
		return eval::Eval(exps->expression.get());
	} else if (type == "BooleanExpression") {
		bool truthy = ((BooleanExpression *)node)->value;
		if (truthy) {
			return object_constant::TRUE_OBJ;
		}
		return object_constant::FALSE_OBJ;
	} else if (type == "PrefixExpression") {
		auto pre = (PrefixExpression *)node;
		auto right = Eval(pre->right.get());
		return eval::eval_prefix_expression(pre->opr, right);
	} else if (type == "InfixExpression") {
		auto inf = (InfixExpression *)node;
		auto right = eval::Eval(inf->right.get());
		auto left = eval::Eval(inf->left.get());

		return eval::eval_infix_exp(inf->opr, right, left);
	}

	return nullptr;
}

Object *eval::eval_statements(Node *program) {
	Object *res;
	auto pg = dynamic_cast<Program *>(program);

	for (auto &st : pg->statements) {
		res = eval::Eval(st.get());
	}

	return res;
}

Object *eval::eval_prefix_expression(std::string opr, Object *right) {
	if (opr == "!") {
		return eval::eval_bang_exp(right);
	} else if (opr == "-") {
		return eval::eval_minus_exp(right);
	}

	return object_constant::null;
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
		Integer* obj = new Integer(-((Integer*)right)->value);
		return obj;
	}

	return object_constant::null;
}

Object *eval::eval_infix_exp(std::string opr, Object *right,
														 Object *left) {
	if (left->Type() == objecttypes::INTEGER && right->Type() == objecttypes::INTEGER)
		return eval::eval_integer_infix(opr, right, left);
	else if (opr == "==") {
		return eval::boolean_to_object(left == right);
	} else if (opr == "!=") {
		return eval::boolean_to_object(left != right);
	}
	return object_constant::null;
}

Object *eval::eval_integer_infix(std::string opr, Object *right,
														 Object *left) {
	auto left_val = ((Integer*)left)->value;
	auto right_val = ((Integer*)right)->value;

	if (opr == "+") {
		return new Integer(left_val+right_val);
	} else if (opr == "-") {
		return new Integer(left_val-right_val);
	} else if (opr == "*") {
		return new Integer(left_val*right_val);
	} else if (opr == "/") {
		return new Integer(left_val/right_val);
	} else if (opr == "<")
		return eval::boolean_to_object(left_val < right_val);
	else if (opr == ">")
		return eval::boolean_to_object(left_val > right_val);
	else if (opr == "==")
		return eval::boolean_to_object(left_val == right_val);
	else if (opr == "!=")
		return eval::boolean_to_object(left_val != right_val);

	return object_constant::null;
}

Object *eval::boolean_to_object(bool value) {
	return (value ? object_constant::TRUE_OBJ : object_constant::FALSE_OBJ);
}
