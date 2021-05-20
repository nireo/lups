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
		auto exps = dynamic_cast<ExpressionStatement *>(node);
		return eval::Eval(exps->expression.get());
	} else if (type == "BooleanExpression") {
		bool truthy = ((BooleanExpression *)node)->value;
		if (truthy) {
			return object_constant::TRUE_OBJ;
		}

		return object_constant::FALSE_OBJ;
	} else if (type == "PrefixExpression") {
		auto pre = dynamic_cast<PrefixExpression *>(node);
		auto right = Eval(((PrefixExpression *)node)->right.get());
		return eval::eval_prefix_expression(pre->opr, right);
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

Object *eval::eval_prefix_expression(const std::string &opr, Object *right) {
	if (opr == "!") {
		return eval::eval_bang_exp(right);
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
