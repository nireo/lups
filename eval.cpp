#include "eval.h"
#include "ast.h"
#include "object.h"

std::unique_ptr<Object> eval::Eval(std::unique_ptr<Node> node) {
	auto type = node->Type();
	if (type == "IntegerLiteral") {
		auto obj = std::make_unique<Integer>();
		auto intl = dynamic_cast<IntegerLiteral *>(obj.get());
		obj->value = intl->value;
		free(intl);

		return obj;
	} else if (type == "Program") {
		// eval statements
	} else if (type == "ExpressionStatement") {
		auto obj = std::make_unique<Integer>();
		auto exps = dynamic_cast<ExpressionStatement *>(obj.get());

		return eval::Eval(std::move(exps->expression));
	}

	return nullptr;
}
