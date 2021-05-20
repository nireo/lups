#include "eval.h"
#include "ast.h"
#include "object.h"

namespace object_constant {
	Null* null = new Null();
}

Object* eval::Eval(Node* node) {
	auto type = node->Type();
	if (type == "IntegerLiteral") {
		Integer* obj = new Integer(((IntegerLiteral*)node)->value);
		return obj;
	} else if (type == "Program") {
		return eval::eval_statements(node);
	} else if (type == "ExpressionStatement") {
		auto exps = dynamic_cast<ExpressionStatement *>(node);
		return eval::Eval(exps->expression.get());
	} else if (type == "BooleanExpression") {
		Boolean* obj = new Boolean(((BooleanExpression*)node)->value);
		return obj;
	}

	return nullptr;
}

Object* eval::eval_statements(Node* program) {
	Object* res;
	auto pg = dynamic_cast<Program *>(program);

	for (auto& st : pg->statements) {
		res = eval::Eval(st.get());
	}

	return res;
}
