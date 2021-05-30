#include "compiler.h"
#include "ast.h"
#include "code.h"
#include "object.h"
#include <vector>

int Compiler::compile(Node *node) {
	auto type = node->Type();
	if (type == "Program") {
		for (const auto &statement : ((Program *)node)->statements) {
			auto status = compile(statement.get());
			if (status != 0)
				return status;
		}
	} else if (type == "ExpressionStatement") {
		auto status = compile(((ExpressionStatement *)node)->expression.get());
		if (status != 0)
			return status;
		emit(code::OpPop);
	} else if (type == "InfixExpression") {
		if (((InfixExpression *)node)->opr == "<") {
			// this is the same as the code below, but it adds the infix expressions
			// in a different order such that we only need one type of greater than
			// opcode.
			auto status = compile(((InfixExpression *)node)->right.get());
			if (status != 0)
				return status;

			status = compile(((InfixExpression *)node)->left.get());
			if (status != 0)
				return status;

			emit(code::OpGreaterThan);
			return 0;
		}

		auto status = compile(((InfixExpression *)node)->left.get());
		if (status != 0)
			return status;

		status = compile(((InfixExpression *)node)->right.get());
		if (status != 0)
			return status;

		auto opr = ((InfixExpression *)node)->opr;
		if (opr == "+")
			emit(code::OpAdd);
		else if (opr == "-")
			emit(code::OpSub);
		else if (opr == "*")
			emit(code::OpMul);
		else if (opr == "/")
			emit(code::OpDiv);
		else if (opr == ">") {
			emit(code::OpGreaterThan);
		} else if (opr == "==") {
			emit(code::OpEqual);
		} else if (opr == "!=") {
			emit(code::OpNotEqual);
		} else
			return -1;
	} else if (type == "IntegerLiteral") {
		auto integer = new Integer(((IntegerLiteral *)node)->value);
		emit(code::OpConstant, std::vector<int>{add_constant(integer)});
	} else if (type == "BooleanExpression") {
		auto value = ((BooleanExpression *)node)->value;
		if (value)
			emit(code::OpTrue);
		else
			emit(code::OpFalse);
	} else if (type == "PrefixExpression") {
		auto prex = dynamic_cast<PrefixExpression*>(node);
		auto status = compile(prex->right.get());
		if (status != 0)
			return status;

		if (prex->opr == "!")
			emit(code::OpBang);
		else if (prex->opr == "-")
			emit(code::OpMinus);
		else
			return -1;
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

int Compiler::emit(code::Opcode op) {
	auto inst = code::make(op, std::vector<int>{});
	auto pos = add_instruction(inst);
	return pos;
}


int Compiler::add_instruction(std::vector<char> inst) {
	auto new_inst_pos = m_instructions.size();
	m_instructions.insert(m_instructions.end(), inst.begin(), inst.end());
	return new_inst_pos;
}
