#ifndef LUPS_COMPILER_H
#define LUPS_COMPILER_H

#include "builtins.h"
#include "code.h"
#include "object.h"
#include <optional>
#include <unordered_map>

typedef std::string SymbolScope;

namespace scopes {
static const SymbolScope GlobalScope = "GLOBAL";
static const SymbolScope LocalScope = "LOCAL";
static const SymbolScope BuiltinScope = "BUITLIN";
static const SymbolScope FreeScope = "FREE";
} // namespace scopes

struct Bytecode {
	code::Instructions instructions;
	std::vector<Object *> constants;
};

struct EmittedInstruction {
	code::Opcode op;
	int pos;
};

struct CompilationScope {
	code::Instructions instructions;
	EmittedInstruction last_inst;
	EmittedInstruction prev_inst;
};

struct Symbol {
	std::string name;
	SymbolScope scope;
	int index;
};

class SymbolTable {
public:
	SymbolTable() {
		definition_num_ = 0;
		store_ = std::unordered_map<std::string, std::unique_ptr<Symbol>>();
		outer_ = nullptr;
		free_symbols_ = std::vector<Symbol>();
	}

	SymbolTable(SymbolTable *outer) {
		definition_num_ = 0;
		store_ = std::unordered_map<std::string, std::unique_ptr<Symbol>>();
		outer_ = outer;
	}

	const Symbol &define(const std::string &name);
	const Symbol &define_free(const Symbol &symbol);
	const Symbol &define_builtin(int index, const std::string &name);
	std::optional<Symbol> resolve(const std::string &name);

	int definition_num_;
	SymbolTable *outer_;

	std::unordered_map<std::string, std::unique_ptr<Symbol>> store_;
	std::vector<Symbol> free_symbols_;
};

class Compiler {
public:
	Compiler();

	// The main compiling function. It returns an optional string containing an
	// error. so if compilation was successful then compile(node).has_value() ==
	// false
	std::optional<std::string> compile(const Node &node);

	int add_constant(Object *obj);
	int emit(code::Opcode op, std::vector<int> operands);
	int emit(code::Opcode op);
	int add_instruction(std::vector<char> inst);
	void set_last_instruction(code::Opcode, int pos);
	bool last_instruction_is(const code::Opcode &op);
	void remove_last_pop();
	void replace_instructions(int pos, const code::Instructions &new_inst);
	void change_operand(int op_pos, int operand);
	code::Instructions current_instructions();
	int add_instructions(std::vector<char> &inst);
	Bytecode *bytecode() {
		return new Bytecode{current_instructions(), constants_};
	}

	void replace_last_pop_with_return();

	code::Instructions &scoped_inst() { return curr_scope().instructions; }
	CompilationScope &curr_scope() { return scopes_.back(); }

	void load_symbol(const Symbol &m);

	void enter_scope();
	code::Instructions leave_scope();

	std::vector<CompilationScope> scopes_;
	int scope_index_;
	SymbolTable *symbol_table_;

private:
	code::Instructions instructions_;
	std::vector<Object *> constants_;

	EmittedInstruction *last_inst_;
	EmittedInstruction *prev_inst_;
};

#endif
