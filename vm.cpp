#include "vm.h"
#include "code.h"
#include "compiler.h"
#include "object.h"

VM::VM(Bytecode *bytecode) {
	m_sp = 0;
	m_instructions = bytecode->instructions;
	m_constants = bytecode->constants;
	m_stack = std::vector<Object *>(2048, nullptr);
}

Object *VM::stack_top() {
	if (m_sp == 0)
		return nullptr;

	return m_stack[m_sp - 1];
}

int VM::run() {
	for (int ip = 0; ip < (int)m_instructions.size(); ++ip) {
		code::Opcode op = m_instructions[ip];

		switch (op) {
		case code::OpConstant: {
			auto const_index = code::decode_uint16(code::Instructions(
					m_instructions.begin() + ip + 1, m_instructions.end()));
			ip += 2;

			int status = push(m_constants[const_index]);
			if (status != 0)
				return status;

			break;
		}
		case code::OpPop: {
			pop();
			break;
		}
		case code::OpMul:
		case code::OpDiv:
		case code::OpSub:
		case code::OpAdd: {
			auto status = execute_binary_operation(op);
			if (status != 0)
				return status;
			break;
		}
		}
	}

	return 0;
}

int VM::push(Object *obj) {
	if (m_sp >= StackSize)
		// stack overflow
		return -1;

	m_stack[m_sp] = obj;
	++m_sp;

	return 0;
}

Object *VM::pop() {
	auto obj = m_stack[m_sp - 1];
	m_sp--;

	return obj;
}

Object *VM::last_popped_stack_elem() {
	return m_stack[m_sp];
}

int VM::execute_binary_operation(code::Opcode op) {
	auto right = pop();
	auto left = pop();

	if (left->Type() == objecttypes::INTEGER && right->Type() == objecttypes::INTEGER) {
		return execute_binary_integer_operation(op, left, right);
	} else {
		return -1;
	}
}

int VM::execute_binary_integer_operation(code::Opcode op, Object *left, Object *right) {
	auto left_val = ((Integer*)left)->value;
	auto right_val = ((Integer*)right)->value;

	int res;
	switch (op) {
	case code::OpAdd:
		res = left_val + right_val;
	case code::OpSub:
		res = left_val - right_val;
	case code::OpMul:
		res = left_val * right_val;
	case code::OpDiv:
		res = left_val / right_val;
	default:
		// operator not recognized
		return -1;
	}

	return push(new Integer(res));
}
