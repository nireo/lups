#include "compiler.h"
#include "ast.h"
#include "code.h"
#include "object.h"
#include <vector>

int Compiler::compile(Node *node) {
	auto type = node->Type();
	if (type == "Program") {
		for (const auto& statement : ((Program*)node)->statements) {
			auto status = compile(statement.get());
			if (status != 0)
				return status;
		}
	} else if (type == "ExpressionStatement") {
		auto status = compile(((ExpressionStatement*)node)->expression.get());
		if (status != 0)
			return status;
	} else if (type == "InfixExpression") {
		auto status = compile(((InfixExpression*)node)->left.get());
		if (status != 0)
			return status;

		status = compile(((InfixExpression*)node)->right.get());
		if (status != 0)
			return status;
	} else if (type == "IntegerLiteral") {
		auto integer = new Integer(((IntegerLiteral*)node)->value);
		emit(code::OpConstant, std::vector<int>{add_constant(integer)});
	}

	return 0;
}

int Compiler::add_constant(Object *obj) {
	m_constants.push_back(obj);
	return (int)m_constants.size() - 1;
}

int Compiler::emit(code::Opcode op, std::vector<int> operands) {
	auto inst = code::make(op, operands);
	auto pos = add_instruction(inst);
	return pos;
}

int Compiler::add_instruction(std::vector<char> inst) {
	auto new_inst_pos = m_instructions.size();
	m_instructions.insert(m_instructions.end(), inst.begin(), inst.end());
	return new_inst_pos;
}
