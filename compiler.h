#ifndef LUPS_COMPILER_H
#define LUPS_COMPILER_H

#include "code.h"
#include "object.h"
#include <unordered_map>

typedef std::string SymbolScope;

namespace scopes {
const SymbolScope GlobalScope = "GLOBAL";
const SymbolScope LocalScope = "LOCAL";
}

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
	~SymbolTable() {
		for (auto pr : store_)
			delete pr.second;
	}

	SymbolTable() {
		definition_num_ = 0;
		store_ = std::unordered_map<std::string, Symbol*>();
		outer_ = nullptr;
	}

	SymbolTable(SymbolTable *outer) {
		definition_num_ = 0;
		store_ = std::unordered_map<std::string, Symbol*>();
		outer_ = outer;
	}

	Symbol *define(const std::string &name);
	Symbol *resolve(const std::string &name);

	int definition_num_;
	std::unordered_map<std::string, Symbol*> store_;
	SymbolTable *outer_;
};

class Compiler {
public:
	Compiler() {
		m_instructions = code::Instructions();
		m_constants = std::vector<Object *>();
		last_inst = nullptr;
		prev_inst = nullptr;
		m_symbol_table = new SymbolTable();
		scope_index = 0;

		auto main_scope = CompilationScope{
			code::Instructions(),
			EmittedInstruction{},
			EmittedInstruction{},
		};

		scopes = std::vector<CompilationScope>(1, main_scope);
	}

	// int is the statuscode
	int compile(Node *node);
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
	Bytecode *bytecode() { return new Bytecode{current_instructions(), m_constants}; }

	void replace_last_pop_with_return();

	code::Instructions& scoped_inst() { return curr_scope().instructions; }
	CompilationScope& curr_scope() { return scopes.back(); }

	void enter_scope();
	code::Instructions leave_scope();

	std::vector<CompilationScope> scopes;
	int scope_index;
	SymbolTable *m_symbol_table;
private:
	code::Instructions m_instructions;
	std::vector<Object *> m_constants;

	EmittedInstruction *last_inst;
	EmittedInstruction *prev_inst;
};

#endif
