#ifndef LUPS_VM_H
#define LUPS_VM_H

#include "code.h"
#include "compiler.h"
constexpr int StackSize = 2048;
constexpr int GlobalsSize = 65536;

class VM {
public:
	VM(Bytecode *bytecode);
	Object *stack_top();
	int run();
	int push(Object *obj);
	Object *pop();
	Object *last_popped_stack_elem();

	// binary operations
	int execute_binary_operation(code::Opcode op);
	int execute_binary_integer_operation(code::Opcode op, Object *left, Object *right);
	int execute_binary_string_operation(code::Opcode op, Object *left, Object *right);

	// comparisons
	int execute_comparison(code::Opcode op);
	int execute_integer_comparison(code::Opcode op, Object *left, Object *right);

	// prefix expressions
	int execute_bang_operator();
	int execute_minus_operator();
private:
	int m_sp;
	code::Instructions m_instructions;
	std::vector<Object *> m_constants;
	std::vector<Object *> m_stack;
	std::vector<Object *> m_globals;
};

#endif
