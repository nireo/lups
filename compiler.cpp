#include "compiler.h"
#include "ast.h"
#include "code.h"
#include "object.h"
#include <algorithm>
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
		emit(code::OpConstant, {add_constant(integer)});
	} else if (type == "BooleanExpression") {
		auto value = ((BooleanExpression *)node)->value;
		if (value)
			emit(code::OpTrue);
		else
			emit(code::OpFalse);
	} else if (type == "PrefixExpression") {
		auto prex = dynamic_cast<PrefixExpression *>(node);
		auto status = compile(prex->right.get());
		if (status != 0)
			return status;

		if (prex->opr == "!")
			emit(code::OpBang);
		else if (prex->opr == "-")
			emit(code::OpMinus);
		else
			return -1;
	} else if (type == "IfExpression") {
		auto ifx = dynamic_cast<IfExpression *>(node);
		auto status = compile(ifx->cond.get());
		if (status != 0)
			return status;

		auto jump_not_truthy_pos =
				emit(code::OpJumpNotTruthy, {9999});

		status = compile(ifx->after.get());
		if (status != 0)
			return status;

		if (is_last_inst_pop())
			remove_last_pop();

		auto jump_pos = emit(code::OpJump, {9999});
		auto after_conq_pos = m_instructions.size();
		change_operand(jump_not_truthy_pos, after_conq_pos);

		if (ifx->other == nullptr) {
			emit(code::OpNull);
		} else {
			status = compile(ifx->other.get());
			if (status != 0)
				return status;

			if (is_last_inst_pop())
				remove_last_pop();
		}
		auto after_other_pos = m_instructions.size();
		change_operand(jump_pos, after_other_pos);
	} else if (type == "BlockExpression") {
		for (auto &st : ((BlockStatement *)node)->statements) {
			auto status = compile(st.get());
			if (status != 0)
				return status;
		}
	} else if (type == "LetStatement") {
		auto letexp = dynamic_cast<LetStatement*>(node);
		auto status = compile(letexp->value.get());
		if (status != 0)
			return status;

		auto symbol = m_symbol_table->define(letexp->name->value);
		emit(code::OpSetGlobal, {symbol->index});
	} else if (type == "Identifier") {
		auto symbol = m_symbol_table->resolve(((Identifier*)node)->value);
		if (symbol == nullptr)
			return -1;
		emit(code::OpGetGlobal, {symbol->index});
	} else if (type == "StringLiteral") {
		auto str = new String(((StringLiteral*)node)->TokenLiteral());
		emit(code::OpConstant, {add_constant(str)});
	} else if (type == "ArrayLiteral") {
		for (const auto &el : ((ArrayLiteral*)node)->elements) {
			auto status = compile(el.get());
			if (status != 0)
				return status;
		}

		emit(code::OpArray, {(int)((ArrayLiteral*)node)->elements.size()});
	} else if (type == "HashLiteral") {
		auto hash_lit = dynamic_cast<HashLiteral*>(node);

		// TODO: probably sort the elements or maybe not :D
		for (auto const &pr : hash_lit->pairs) {
			auto status = compile(pr.first.get());
			if (status != 0)
				return status;

			status = compile(pr.second.get());
			if (status != 0)
				return status;
		}

		emit(code::OpHash, {(int)hash_lit->pairs.size() * 2});
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
	set_last_instruction(op, pos);

	return pos;
}

int Compiler::emit(code::Opcode op) {
	auto inst = code::make(op, {});
	auto pos = add_instruction(inst);
	set_last_instruction(op, pos);

	return pos;
}

int Compiler::add_instruction(std::vector<char> inst) {
	auto new_inst_pos = m_instructions.size();
	m_instructions.insert(m_instructions.end(), inst.begin(), inst.end());

	return new_inst_pos;
}

void Compiler::set_last_instruction(code::Opcode op, int pos) {
	auto prev = last_inst;
	auto last = new EmittedInstruction{op, pos};

	prev_inst = prev;
	last_inst = last;
}

bool Compiler::is_last_inst_pop() { return last_inst->op == code::OpPop; }

void Compiler::remove_last_pop() {
	m_instructions = code::Instructions(m_instructions.begin(),
																			m_instructions.begin() + last_inst->pos);

	last_inst = prev_inst;
}

void Compiler::change_operand(int op_pos, int operand) {
	auto op = m_instructions[op_pos];
	auto new_inst = code::make(op, {operand});

	replace_instructions(op_pos, new_inst);
}

void Compiler::replace_instructions(int pos, code::Instructions inst) {
	for (int i = 0; i < (int)inst.size(); ++i) {
		m_instructions[pos + i] = inst[i];
	}
}

Symbol *SymbolTable::define(const std::string &name) {
	auto symbol = new Symbol{name, scopes::GlobalScope, m_definition_num};
	m_store[name] = symbol;
	++m_definition_num;

	return symbol;
}

Symbol *SymbolTable::resolve(const std::string &name) {
	if (m_store.count(name) == 0)
		return nullptr;

	return m_store[name];
}
