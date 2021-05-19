#include "eval.h"
#include "ast.h"
#include "object.h"

Object* eval::Eval(Node* node) {
	auto type = node->Type();
	if (type == "IntegerLiteral") {
		Integer* obj;
		auto intobj = dynamic_cast<IntegerLiteral *>(node);
		obj->value = intobj->value;

		return obj;
	} else if (type == "Program") {
		return eval::eval_statements(node);
	} else if (type == "ExpressionStatement") {
		auto exps = dynamic_cast<ExpressionStatement *>(node);
		return eval::Eval(exps->expression.get());
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
