#include "compiler.h"
#include "ast.h"
#include "code.h"
#include "object.h"
#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

std::optional<std::string> Compiler::compile(const Node &node) {
	AstType type = node.Type();
	switch (type) {
	case AstType::Program: {
		try {
			const auto &prg = dynamic_cast<const Program &>(node);
			for (const auto &statement : prg.statements) {
				auto status = compile(*statement);
				if (status.has_value())
					return status;
			}
		} catch (std::bad_cast &e) {
			return "could not cast program type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::ExpressionStatement: {
		try {
			const auto &exp_stmt = dynamic_cast<const ExpressionStatement &>(node);
			auto status = compile(*exp_stmt.expression);
			if (status.has_value())
				return "error while compiling expression statement expression.";
			emit(code::OpPop);
		} catch (std::bad_cast &e) {
			return "could not cast expression statement type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::InfixExpression: {
		try {
			const auto &infx_exp = dynamic_cast<const InfixExpression &>(node);
			if (infx_exp.opr == "<") {
				// this is the same as the code below, but it adds the infix expressions
				// in a different order such that we only need one type of greater than
				// opcode.
				auto status = compile(*infx_exp.right);
				if (status.has_value())
					return "error compiling right side of infix expression";

				status = compile(*infx_exp.left);
				if (status.has_value())
					return "error compiling the left side of infix expression";

				emit(code::OpGreaterThan);
				return std::nullopt;
			}
			auto status = compile(*infx_exp.left);
			if (status.has_value())
				return "error compiling the left side of infix expression";

			status = compile(*infx_exp.right);
			if (status.has_value())
				return "error compiling the right side of infix expression";

			const auto &opr = infx_exp.opr;
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
				return "error: unrecognized infix operation";
		} catch (std::bad_cast &e) {
			return "could not cast infix expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::IntegerLiteral: {
		try {
			const auto &intl = dynamic_cast<const IntegerLiteral &>(node);
			auto integer = new Integer(intl.value);
			emit(code::OpConstant, {add_constant(integer)});
		} catch (std::bad_cast &e) {
			return "could not cast integer literal type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::BooleanExpression: {
		try {
			const auto &booll = dynamic_cast<const BooleanExpression &>(node);
			if (booll.value)
				emit(code::OpTrue);
			else
				emit(code::OpFalse);
		} catch (std::bad_cast &e) {
			return "could not cast boolean expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::PrefixExpression: {
		try {
			const auto &prex = dynamic_cast<const PrefixExpression &>(node);
			auto status = compile(*prex.right);
			if (status.has_value())
				return "error compiling the right side of prefix expression";

			if (prex.opr == "!")
				emit(code::OpBang);
			else if (prex.opr == "-")
				emit(code::OpMinus);
			else
				return "error: unrecognized prefix operation";
		} catch (std::bad_cast &e) {
			return "could not cast prefix expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::IfExpression: {
		try {
			const auto &ifx = dynamic_cast<const IfExpression &>(node);
			auto status = compile(*ifx.cond);
			if (status.has_value())
				return "error compiling the if expression conditional";

			const auto jump_not_truthy_pos = emit(code::OpJumpNotTruthy, {9999});

			status = compile(*ifx.after);
			if (status.has_value())
				return "error compiling if expression true body";

			if (last_instruction_is(code::OpPop))
				remove_last_pop();

			const auto jump_pos = emit(code::OpJump, {9999});
			const auto after_conq_pos = current_instructions().size();
			change_operand(jump_not_truthy_pos, after_conq_pos);

			if (ifx.other == nullptr) {
				emit(code::OpNull);
			} else {
				status = compile(*ifx.other);
				if (status.has_value())
					return "error compiling if expression false body";

				if (last_instruction_is(code::OpPop))
					remove_last_pop();
			}
			const auto after_other_pos = current_instructions().size();
			change_operand(jump_pos, after_other_pos);
		} catch (std::bad_cast &e) {
			return "could not cast if expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::BlockStatement: {
		try {
			const auto &block = dynamic_cast<const BlockStatement &>(node);
			for (auto &st : block.statements) {
				const auto status = compile(*st);
				if (status.has_value())
					return "error compiling block statement";
			}
		} catch (std::bad_cast &e) {
			return "could not cast block statement type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::LetStatement: {
		try {
			const auto &letexp = dynamic_cast<const LetStatement &>(node);
			const auto &symbol = symbol_table_->define(letexp.name->value);

			const auto status = compile(*letexp.value);
			if (status.has_value())
				return "error compiling let statement";

			if (symbol.scope == scopes::GlobalScope)
				emit(code::OpSetGlobal, {symbol.index});
			else
				emit(code::OpSetLocal, {symbol.index});
		} catch (std::bad_cast &e) {
			return "could not cast let statement type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::Identifier: {
		try {
			const auto &identifier = dynamic_cast<const Identifier &>(node);
			auto symbol = symbol_table_->resolve(identifier.value);
			if (!symbol.has_value())
				return "Symbol is not found in symbol table.";

			load_symbol(symbol.value());
		} catch (std::bad_cast &e) {
			return "could not cast identifier type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::StringLiteral: {
		try {
			const auto &strl = dynamic_cast<const StringLiteral &>(node);
			const auto str = new String(strl.TokenLiteral());
			emit(code::OpConstant, {add_constant(str)});
		} catch (std::bad_cast &e) {
			return "could not cast string literal type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::ArrayLiteral: {
		try {
			const auto &arrlit = dynamic_cast<const ArrayLiteral &>(node);
			for (const auto &el : arrlit.elements) {
				const auto status = compile(*el);
				if (status.has_value())
					return "error compiling array element";
			}

			emit(code::OpArray, {(int)arrlit.elements.size()});
		} catch (std::bad_cast &e) {
			return "could not cast array literal type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::HashLiteral: {
		try {
			const auto &hash_lit = dynamic_cast<const HashLiteral &>(node);

			// TODO: probably sort the elements or maybe not it works either way
			for (auto const &pr : hash_lit.pairs) {
				auto status = compile(*pr.first);
				if (status.has_value())
					return "error compiling first part of hash pair";

				status = compile(*pr.second);
				if (status.has_value())
					return "error compiling second part of hash pair";
			}

			emit(code::OpHash, {(int)hash_lit.pairs.size() * 2});
		} catch (std::bad_cast &e) {
			return "could not cast hash literal type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::IndexExpression: {
		try {
			const auto &index_expression =
					dynamic_cast<const IndexExpression &>(node);
			auto status = compile(*index_expression.left);
			if (status.has_value())
				return "error compiling index expression left side";

			status = compile(*index_expression.index);
			if (status.has_value())
				return "error compiling index expression index";
			emit(code::OpIndex);
		} catch (std::bad_cast &e) {
			return "could not cast index expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::FunctionLiteral: {
		try {
			enter_scope();
			const auto &func = dynamic_cast<const FunctionLiteral &>(node);
			for (const auto &pr : func.params)
				symbol_table_->define(pr->value);

			auto status = compile(*func.body);
			if (status.has_value())
				return "error compiling function body";

			if (last_instruction_is(code::OpPop))
				replace_last_pop_with_return();

			if (!last_instruction_is(code::OpReturnValue))
				emit(code::OpReturn);

			const auto num_locals = symbol_table_->definition_num_;
			const auto &free_symbols = symbol_table_->free_symbols_;
			auto instructions = leave_scope();

			for (const auto &sym : free_symbols)
				load_symbol(sym);

			auto compiled_function = new CompiledFunction(instructions, num_locals);
			compiled_function->m_num_parameters = func.params.size();

			auto fn_index = add_constant(compiled_function);

			emit(code::OpClosure, {fn_index, (int)free_symbols.size()});
		} catch (std::bad_cast &e) {
			return "could not cast index expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::ReturnStatement: {
		try {
			const auto &ret = dynamic_cast<const ReturnStatement&>(node);
		const auto status = compile(*ret.return_value);
		if (status.has_value())
			return "error compiling return value";
		emit(code::OpReturnValue);
		} catch (std::bad_cast &e) {
			return "could not cast return statement type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	case AstType::CallExpression: {
		try {
			const auto& call_exp = dynamic_cast<const CallExpression&>(node);
			auto status = compile(*call_exp.func);
			if (status.has_value())
				return "error compiling call function";

			for (const auto &arg : call_exp.arguments) {
				status = compile(*arg);
				if (status.has_value())
					return "error compiling call argument";
			}

			emit(code::OpCall, {(int)call_exp.arguments.size()});
		} catch (std::bad_cast &e) {
			return "could not cast index expression type to node reference. " +
						 std::string(e.what());
		}
		break;
	}
	}

	return std::nullopt;
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

std::optional<Symbol> SymbolTable::resolve(const std::string &name) {
	const auto iter = store_.find(name);
	if (iter != store_.end())
		return *iter->second;

	if (outer_ == nullptr)
		return std::nullopt;

	auto res = outer_->resolve(name);
	if (!res.has_value())
		return std::nullopt;

	if (res->scope == scopes::GlobalScope || res->scope == scopes::BuiltinScope)
		return res;

	auto free = define_free(res.value());
	return free;
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

	symbol_table_ = new SymbolTable(symbol_table_);
}

code::Instructions Compiler::leave_scope() {
	auto instructions = current_instructions();

	scopes = std::vector<CompilationScope>(scopes.begin(),
																				 scopes.begin() + scopes.size() - 1);
	--scope_index;

	symbol_table_ = symbol_table_->outer_;

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

void Compiler::load_symbol(const Symbol &sm) {
	if (sm.scope == scopes::GlobalScope)
		emit(code::OpGetGlobal, {sm.index});
	else if (sm.scope == scopes::LocalScope)
		emit(code::OpGetLocal, {sm.index});
	else if (sm.scope == scopes::BuiltinScope)
		emit(code::OpGetBuiltin, {sm.index});
	else if (sm.scope == scopes::FreeScope)
		emit(code::OpGetFree, {sm.index});
}

const Symbol &SymbolTable::define_free(const Symbol &org) {
	free_symbols_.push_back(org);

	std::unique_ptr<Symbol> symbol(
			new Symbol{org.name, scopes::FreeScope, (int)free_symbols_.size() - 1});
	store_[org.name] = std::move(symbol);

	return *store_[org.name];
}
