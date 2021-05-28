#include "vm.h"
#include "code.h"
#include "compiler.h"

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
		case code::OpConstant:
			auto const_index = code::decode_uint16(code::Instructions(
					m_instructions.begin() + ip + 1, m_instructions.end()));
			ip += 2;

			int status = push(m_constants[const_index]);
			if (status != 0)
				return status;
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
