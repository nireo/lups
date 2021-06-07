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
		const auto prex = dynamic_cast<PrefixExpression *>(node);
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
		const auto ifx = dynamic_cast<IfExpression *>(node);
		auto status = compile(ifx->cond.get());
		if (status != 0)
			return status;

		const auto jump_not_truthy_pos =
				emit(code::OpJumpNotTruthy, {9999});

		status = compile(ifx->after.get());
		if (status != 0)
			return status;

		if (is_last_inst_pop())
			remove_last_pop();

		const auto jump_pos = emit(code::OpJump, {9999});
		const auto after_conq_pos = current_instructions().size();
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
		const auto after_other_pos = current_instructions().size();
		change_operand(jump_pos, after_other_pos);
	} else if (type == "BlockExpression") {
		for (auto &st : ((BlockStatement *)node)->statements) {
			auto status = compile(st.get());
			if (status != 0)
				return status;
		}
	} else if (type == "LetStatement") {
		const auto letexp = dynamic_cast<LetStatement*>(node);
		const auto status = compile(letexp->value.get());
		if (status != 0)
			return status;

		const auto symbol = m_symbol_table->define(letexp->name->value);
		emit(code::OpSetGlobal, {symbol->index});
	} else if (type == "Identifier") {
		const auto symbol = m_symbol_table->resolve(((Identifier*)node)->value);
		if (symbol == nullptr)
			return -1;
		emit(code::OpGetGlobal, {symbol->index});
	} else if (type == "StringLiteral") {
		const auto str = new String(((StringLiteral*)node)->TokenLiteral());
		emit(code::OpConstant, {add_constant(str)});
	} else if (type == "ArrayLiteral") {
		for (const auto &el : ((ArrayLiteral*)node)->elements) {
			const auto status = compile(el.get());
			if (status != 0)
				return status;
		}

		emit(code::OpArray, {(int)((ArrayLiteral*)node)->elements.size()});
	} else if (type == "HashLiteral") {
		auto hash_lit = dynamic_cast<HashLiteral*>(node);

		// TODO: probably sort the elements or maybe not it works either way
		for (auto const &pr : hash_lit->pairs) {
			auto status = compile(pr.first.get());
			if (status != 0)
				return status;

			status = compile(pr.second.get());
			if (status != 0)
				return status;
		}

		emit(code::OpHash, {(int)hash_lit->pairs.size() * 2});
	} else if (type == "IndexExpression") {
		auto index_expression = dynamic_cast<IndexExpression*>(node);
		auto status = compile(index_expression->left.get());
		if (status != 0)
			return status;

		status = compile(index_expression->index.get());
		if (status != 0)
			return status;
		emit(code::OpIndex);
	} else if (type == "FunctionLiteral") {
		enter_scope();

		auto func = dynamic_cast<FunctionLiteral*>(node);
		auto status = compile(func->body.get());
		if (status != 0)
			return status;

		auto instructions = leave_scope();
		auto compiled_fucntion = new CompiledFunction(instructions);
		emit(code::OpConstant, {add_constant(compiled_fucntion)});
	} else if (type == "ReturnStatement") {
		auto status = compile(((ReturnStatement*)node)->return_value.get());
		if (status != 0)
			return status;
		emit(code::OpReturnValue);
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
	auto cur_inst = current_instructions();
	auto pos_new_instruction = cur_inst.size();
	cur_inst.insert(cur_inst.end(), inst.begin(), inst.end());

	scopes[scope_index].instructions = cur_inst;

	return pos_new_instruction;
}

void Compiler::set_last_instruction(code::Opcode op, int pos) {
	auto prev = scopes[scope_index].last_inst;
	auto last = EmittedInstruction{op, pos};

	scopes[scope_index].prev_inst = prev;
	scopes[scope_index].last_inst = last;
}

bool Compiler::is_last_inst_pop() {
	return scopes[scope_index].last_inst.op == code::OpPop;
}

void Compiler::remove_last_pop() {
	auto last = scopes[scope_index].last_inst;
	auto prev = scopes[scope_index].prev_inst;

	auto old = current_instructions();
	auto new_inst = code::Instructions(old.begin(), old.begin()+last.pos);

	scopes[scope_index].instructions = new_inst;
	scopes[scope_index].last_inst = prev;
}

void Compiler::change_operand(int op_pos, int operand) {
	auto op = current_instructions()[op_pos];
	auto new_inst = code::make(op, {operand});

	replace_instructions(op_pos, new_inst);
}

void Compiler::replace_instructions(int pos, code::Instructions &new_inst) {
	auto& ins = scoped_inst();
	for (int i = 0; i < (int)new_inst.size(); ++i) {
		ins[pos + i] = new_inst[i];
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

code::Instructions Compiler::current_instructions() {
	return scopes[scope_index].instructions;
}

void Compiler::enter_scope() {
	auto scope = CompilationScope{
		code::Instructions(),
		EmittedInstruction{},
		EmittedInstruction{},
	};

	scopes.push_back(scope);
	++scope_index;
}

code::Instructions Compiler::leave_scope() {
	auto instructions = current_instructions();

	scopes = std::vector<CompilationScope>(scopes.begin(), scopes.begin()+scopes.size()-1);
	--scope_index;

	return instructions;
}
