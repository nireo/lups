#include "vm.h"
#include "code.h"
#include "compiler.h"
#include "eval.h"
#include "object.h"

// TODO: add better error handling rather than just returning integers. Probably by making
// some kind of error class or something since enums aren't different from integers.


Object *native_bool_to_obj(bool val) {
	return (val ? object_constant::TRUE_OBJ : object_constant::FALSE_OBJ);
}

bool is_truthy(Object *obj) {
	if (obj == nullptr)
		return false;

	if (obj->Type() == objecttypes::BOOLEAN)
		return ((Boolean*)obj)->value;
	else if (obj->Type() == objecttypes::NULLOBJ)
		return false;

	return true;
}

// create vm instance from bytecode generated by compiler.
VM::VM(Bytecode *bytecode) {
	m_sp = 0;
	m_instructions = bytecode->instructions;
	m_constants = bytecode->constants;
	m_stack = std::vector<Object *>(StackSize, nullptr);
}

// return the topmost element in stack.
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
		case code::OpTrue: {
			auto status = push(object_constant::TRUE_OBJ);
			if (status != 0)
				return status;
			break;
		}
		case code::OpFalse: {
			auto status = push(object_constant::FALSE_OBJ);
			if (status != 0)
				return status;
			break;
		}
		case code::OpEqual:
		case code::OpNotEqual:
		case code::OpGreaterThan: {
			auto status = execute_comparison(op);
			if (status != 0)
				return status;
			break;
		}
		case code::OpBang: {
			auto status = execute_bang_operator();
			if (status != 0)
				return status;
			break;
		}
		case code::OpMinus: {
			auto status = execute_minus_operator();
			if (status != 0)
				return status;
			break;
		}
		case code::OpJump: {
			auto pos = (int)code::decode_uint16(code::Instructions(
					m_instructions.begin() + ip + 1, m_instructions.begin() + ip + 3));
			ip = pos - 1;
		}
		case code::OpJumpNotTruthy: {
			auto pos = (int)code::decode_uint16(code::Instructions(
					m_instructions.begin() + ip + 1, m_instructions.begin() + ip + 3));
			ip += 2;

			auto condition = pop();
			if (!is_truthy(condition))
				ip = pos - 1;
			break;
		}
		case code::OpNull: {
			auto status = push(object_constant::null);
			if (status != 0)
				return status;
		}
		}
	}

	return 0;
}

// push item to the stack and check for stack overflow.
int VM::push(Object *obj) {
	if (m_sp >= StackSize)
		// stack overflow
		return -1;

	m_stack[m_sp] = obj;
	++m_sp;

	return 0;
}

// pop the top element from the stack and return it.
Object *VM::pop() {
	auto obj = m_stack[m_sp - 1];
	m_sp--;

	return obj;
}

Object *VM::last_popped_stack_elem() { return m_stack[m_sp]; }

int VM::execute_binary_operation(code::Opcode op) {
	auto right = pop();
	auto left = pop();

	// check that types support a certain operation.
	if (left->Type() == objecttypes::INTEGER &&
			right->Type() == objecttypes::INTEGER) {
		return execute_binary_integer_operation(op, left, right);
	} else {
		// the types don't have a supported binary expression
		return -1;
	}
}

int VM::execute_binary_integer_operation(code::Opcode op, Object *left,
																				 Object *right) {
	auto left_val = ((Integer *)left)->value;
	auto right_val = ((Integer *)right)->value;

	int res;
	switch (op) {
	case code::OpAdd:
		res = left_val + right_val;
		break;
	case code::OpSub:
		res = left_val - right_val;
		break;
	case code::OpMul:
		res = left_val * right_val;
		break;
	case code::OpDiv:
		res = left_val / right_val;
		break;
	default:
		// operator not recognized
		return -1;
	}

	return push(new Integer(res));
}

int VM::execute_comparison(code::Opcode op) {
	auto right = pop();
	auto left = pop();

	// check that types support a certain operation.
	if (left->Type() == objecttypes::INTEGER &&
			right->Type() == objecttypes::INTEGER) {
		return execute_integer_comparison(op, left, right);
	}

	switch (op) {
	case code::OpEqual:
		return push(native_bool_to_obj(right == left));
	case code::OpNotEqual:
		return push(native_bool_to_obj(right != left));
	}

	// the operation is not recognized.
	return -1;
}

int VM::execute_integer_comparison(code::Opcode op, Object *left,
																	 Object *right) {
	auto left_value = ((Integer *)left)->value;
	auto right_value = ((Integer *)right)->value;

	switch (op) {
	case code::OpEqual:
		return push(native_bool_to_obj(right_value == left_value));
	case code::OpNotEqual:
		return push(native_bool_to_obj(right_value != left_value));
	case code::OpGreaterThan:
		return push(native_bool_to_obj(left_value > right_value));
	}

	return -1;
}

int VM::execute_bang_operator() {
	auto oper = pop();
	if (oper == object_constant::TRUE_OBJ)
		return push(object_constant::FALSE_OBJ);
	else if (oper == object_constant::FALSE_OBJ)
		return push(object_constant::TRUE_OBJ);
	else if (oper == object_constant::null)
		return push(object_constant::TRUE_OBJ);

	return push(object_constant::FALSE_OBJ);
}

int VM::execute_minus_operator() {
	auto oper = pop();
	if (oper->Type() != objecttypes::INTEGER)
		// type cannot be used in conjunction with integer type
		return -1;

	auto value = ((Integer *)oper)->value;
	return push(new Integer(-value));
}
