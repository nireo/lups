#include "compiler.h"
#include "ast.h"
#include "code.h"
#include "object.h"
#include <algorithm>
#include <vector>

int Compiler::compile(Node *node) {
	AstType type = node->Type();
	switch (type) {
	case AstType::Program: {
		for (const auto &statement : ((Program *)node)->statements) {
			auto status = compile(statement.get());
			if (status != 0)
				return status;
		}
		break;
	}
	case AstType::ExpressionStatement: {
		auto status = compile(((ExpressionStatement *)node)->expression.get());
		if (status != 0)
			return status;
		emit(code::OpPop);
		break;
	}
	case AstType::InfixExpression: {
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
		break;
	}
	case AstType::IntegerLiteral: {
		auto integer = new Integer(((IntegerLiteral *)node)->value);
		emit(code::OpConstant, {add_constant(integer)});
		break;
	}
	case AstType::BooleanExpression: {
		auto value = ((BooleanExpression *)node)->value;
		if (value)
			emit(code::OpTrue);
		else
			emit(code::OpFalse);
		break;
	}
	case AstType::PrefixExpression: {
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
		break;
	}
	case AstType::IfExpression: {
		const auto ifx = dynamic_cast<IfExpression *>(node);
		auto status = compile(ifx->cond.get());
		if (status != 0)
			return status;

		const auto jump_not_truthy_pos = emit(code::OpJumpNotTruthy, {9999});

		status = compile(ifx->after.get());
		if (status != 0)
			return status;

		if (last_instruction_is(code::OpPop))
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

			if (last_instruction_is(code::OpPop))
				remove_last_pop();
		}
		const auto after_other_pos = current_instructions().size();
		change_operand(jump_pos, after_other_pos);
		break;
	}
	case AstType::BlockStatement: {
		for (auto &st : ((BlockStatement *)node)->statements) {
			auto status = compile(st.get());
			if (status != 0)
				return status;
		}
		break;
	}
	case AstType::LetStatement: {
		const auto letexp = dynamic_cast<LetStatement *>(node);
		const auto status = compile(letexp->value.get());
		if (status != 0)
			return status;

		const auto &symbol = m_symbol_table->define(letexp->name->value);
		if (symbol.scope == scopes::GlobalScope)
			emit(code::OpSetGlobal, {symbol.index});
		else
			emit(code::OpSetLocal, {symbol.index});
		break;
	}
	case AstType::Identifier: {
		auto symbol = m_symbol_table->resolve(((Identifier *)node)->value);
		if (symbol == nullptr)
			return -1;

		load_symbol(symbol);
		break;
	}
	case AstType::StringLiteral: {
		const auto str = new String(((StringLiteral *)node)->TokenLiteral());
		emit(code::OpConstant, {add_constant(str)});
		break;
	}
	case AstType::ArrayLiteral: {
		for (const auto &el : ((ArrayLiteral *)node)->elements) {
			const auto status = compile(el.get());
			if (status != 0)
				return status;
		}

		emit(code::OpArray, {(int)((ArrayLiteral *)node)->elements.size()});
		break;
	}
	case AstType::HashLiteral: {
		auto hash_lit = dynamic_cast<HashLiteral *>(node);

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
		break;
	}
	case AstType::IndexExpression: {
		auto index_expression = dynamic_cast<IndexExpression *>(node);
		auto status = compile(index_expression->left.get());
		if (status != 0)
			return status;

		status = compile(index_expression->index.get());
		if (status != 0)
			return status;
		emit(code::OpIndex);
		break;
	}
	case AstType::FunctionLiteral: {
		enter_scope();
		auto func = dynamic_cast<FunctionLiteral *>(node);
		for (const auto &pr : func->params)
			m_symbol_table->define(pr->value);

		auto status = compile(func->body.get());
		if (status != 0)
			return status;

		if (last_instruction_is(code::OpPop))
			replace_last_pop_with_return();

		if (!last_instruction_is(code::OpReturnValue))
			emit(code::OpReturn);

		const auto num_locals = m_symbol_table->definition_num_;
		auto instructions = leave_scope();
		auto compiled_function = new CompiledFunction(instructions, num_locals);
		compiled_function->m_num_parameters = func->params.size();

		auto fn_index = add_constant(compiled_function);

		emit(code::OpClosure, {fn_index, 0});
		break;
	}
	case AstType::ReturnStatement: {
		auto status = compile(((ReturnStatement *)node)->return_value.get());
		if (status != 0)
			return status;
		emit(code::OpReturnValue);
		break;
	}
	case AstType::CallExpression: {
		const auto call_exp = dynamic_cast<CallExpression *>(node);
		auto status = compile(call_exp->func.get());
		if (status != 0)
			return status;

		for (const auto &arg : call_exp->arguments) {
			status = compile(arg.get());
			if (status != 0)
				return status;
		}

		emit(code::OpCall, {(int)call_exp->arguments.size()});
		break;
	}
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

void Compiler::remove_last_pop() {
	auto last = scopes[scope_index].last_inst;
	auto prev = scopes[scope_index].prev_inst;

	auto old = current_instructions();
	auto new_inst = code::Instructions(old.begin(), old.begin() + last.pos);

	scopes[scope_index].instructions = new_inst;
	scopes[scope_index].last_inst = prev;
}

void Compiler::change_operand(int op_pos, int operand) {
	auto op = current_instructions()[op_pos];
	auto new_inst = code::make(op, {operand});

	replace_instructions(op_pos, new_inst);
}

void Compiler::replace_instructions(int pos,
																		const code::Instructions &new_inst) {
	auto &ins = scoped_inst();
	for (int i = 0; i < (int)new_inst.size(); ++i) {
		ins[pos + i] = new_inst[i];
	}
}

const Symbol &SymbolTable::define(const std::string &name) {
	auto symbol = std::make_unique<Symbol>();
	symbol->index = definition_num_;
	symbol->name = name;

	if (outer_ == nullptr) {
		symbol->scope = scopes::GlobalScope;
	} else {
		symbol->scope = scopes::LocalScope;
	}

	store_[name] = std::move(symbol);
	++definition_num_;

	return *store_[name];
}

Symbol *SymbolTable::resolve(const std::string &name) {
	if (store_.count(name) == 0) {
		if (outer_ == nullptr)
			return nullptr;

		return outer_->resolve(name);
	}

	return store_[name].get();
}

const Symbol &SymbolTable::define_builtin(int index, const std::string &name) {
	std::unique_ptr<Symbol> symbol(new Symbol{name, scopes::BuiltinScope, index});
	store_[name] = std::move(symbol);

	return *store_[name];
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

	m_symbol_table = new SymbolTable(m_symbol_table);
}

code::Instructions Compiler::leave_scope() {
	auto instructions = current_instructions();

	scopes = std::vector<CompilationScope>(scopes.begin(),
																				 scopes.begin() + scopes.size() - 1);
	--scope_index;

	m_symbol_table = m_symbol_table->outer_;

	return instructions;
}

bool Compiler::last_instruction_is(const code::Opcode &op) {
	if (scoped_inst().size() == 0)
		return false;

	return scopes[scope_index].last_inst.op == op;
}

void Compiler::replace_last_pop_with_return() {
	auto last_pos = scopes[scope_index].last_inst.pos;
	replace_instructions(last_pos, code::make(code::OpReturnValue, {}));
	scopes[scope_index].last_inst.op = code::OpReturnValue;
}

void Compiler::load_symbol(const Symbol *sm) {
	if (sm->scope == scopes::GlobalScope)
		emit(code::OpGetGlobal, {sm->index});
	else if (sm->scope == scopes::LocalScope)
		emit(code::OpGetLocal, {sm->index});
	else if (sm->scope == scopes::BuiltinScope)
		emit(code::OpGetBuiltin, {sm->index});
}
