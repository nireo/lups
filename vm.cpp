#include "vm.h"
#include "builtins.h"
#include "code.h"
#include "compiler.h"
#include "eval.h"
#include "object.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <map>

// TODO: add better error handling rather than just returning integers. Probably
// by making some kind of error class or something since enums aren't different
// from integers.

// TODO: Start using unique pointers since using raw pointers and returning raw
// pointers is not best practice.

// TODO: After doing some research I think that using std::optional<std::string>
// where the string is the error message is the way to go with proper error
// handling. Since then I don't have to hardcode multiple different errors
// values into enums or something similar.


Object *native_bool_to_obj(bool val) {
	return (val ? object_constant::TRUE_OBJ : object_constant::FALSE_OBJ);
}

bool is_truthy(Object *obj) {
	if (obj == nullptr)
		return false;

	if (obj->Type() == objecttypes::BOOLEAN)
		return ((Boolean *)obj)->value;
	else if (obj->Type() == objecttypes::NULLOBJ)
		return false;

	return true;
}

// create vm instance from bytecode generated by compiler.
VM::VM(Bytecode *bytecode) {
	m_sp = 0;
	m_constants = bytecode->constants;
	m_stack = std::vector<Object *>(StackSize, nullptr);
	m_globals = std::vector<Object *>(GlobalsSize);

	std::unique_ptr<Frame> main_frame(new Frame(bytecode->instructions, 0));

	frames_ = std::array<std::unique_ptr<Frame>, MaxFrames>();
	frames_[0] = std::move(main_frame);

	frames_index_ = 1;
}

// return the topmost element in stack.
Object *VM::stack_top() {
	if (m_sp == 0)
		return nullptr;

	return m_stack[m_sp - 1];
}

int VM::run() {
	for (current_frame().ip_ = 0;
			 current_frame().ip_ < (int)current_frame().instructions().size();
			 ++current_frame().ip_) {
		auto &ip = current_frame().ip_;
		const auto &inst = current_frame().instructions();
		const auto op = inst[ip];

		switch (op) {
		case code::OpConstant: {
			auto const_index = code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ += 2;

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
			auto pos = (int)code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ = pos - 1;
			break;
		}
		case code::OpJumpNotTruthy: {
			auto pos = (int)code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ += 2;

			auto condition = pop();
			if (!is_truthy(condition))
				current_frame().ip_ = pos - 1;
			break;
		}
		case code::OpNull: {
			auto status = push(object_constant::null);
			if (status != 0)
				return status;
			break;
		}
		case code::OpSetGlobal: {
			auto global_index = code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ += 2;

			m_globals[global_index] = pop();
			break;
		}
		case code::OpGetGlobal: {
			auto global_index = code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ += 2;

			auto status = push(m_globals[global_index]);
			if (status != 0)
				return status;
			break;
		}
		case code::OpArray: {
			auto num_elements = code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ += 2;
			auto array = build_array(m_sp - num_elements, m_sp);
			m_sp = m_sp - num_elements;

			auto status = push(array);
			if (status != 0)
				return status;
			break;
		}
		case code::OpHash: {
			auto num_elements = code::decode_uint16(
					code::Instructions(inst.begin() + ip + 1, inst.begin() + ip + 3));
			current_frame().ip_ += 2;

			auto hash = build_hash(m_sp - num_elements, m_sp);
			m_sp = m_sp - num_elements;
			auto status = push(hash);
			if (status != 0)
				return status;
			break;
		}
		case code::OpIndex: {
			auto index = pop();
			auto left = pop();

			auto status = execute_index_expression(left, index);
			if (status != 0)
				return status;
			break;
		}
		case code::OpCall: {
			auto num_args = (int)((std::uint8_t)inst[ip + 1]);
			current_frame().ip_ += 1;

			auto status = execute_call(num_args);
			if (status != 0)
				return status;

			break;
		}
		case code::OpReturnValue: {
			auto return_value = pop();
			const auto &frame = pop_frame();
			m_sp = frame.base_pointer_ - 1;

			auto status = push(return_value);
			if (status != 0)
				return status;
			break;
		}
		case code::OpReturn: {
			const auto &frame = pop_frame();
			m_sp = frame.base_pointer_ - 1;

			auto status = push(object_constant::null);
			if (status != 0)
				return status;
			break;
		}
		case code::OpSetLocal: {
			auto local_index = (int)((std::uint8_t)inst[ip + 1]);
			current_frame().ip_ += 1;

			auto &frame = current_frame();
			m_stack[frame.base_pointer_ + local_index] = pop();
			break;
		}
		case code::OpGetLocal: {
			auto local_index = (int)((std::uint8_t)inst[ip + 1]);
			current_frame().ip_ += 1;

			const auto &frame = current_frame();
			auto status = push(m_stack[frame.base_pointer_ + local_index]);
			if (status != 0) {
				return status;
			}
			break;
		}
		case code::OpGetBuiltin: {
			auto builtin_index = (int)((std::uint8_t)inst[ip+1]);
			current_frame().ip_ += 1;

			// the compiler ensures that this index exists
			auto def = builtin_functions::functions[builtin_index];

			auto status = push(def.second);
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
	} else if (left->Type() == objecttypes::STRING &&
						 right->Type() == objecttypes::STRING) {
		return execute_binary_string_operation(op, left, right);
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

int VM::execute_binary_string_operation(code::Opcode op, Object *left,
																				Object *right) {
	if (op != code::OpAdd)
		return -1;

	auto left_value = ((String *)left)->value;
	auto right_value = ((String *)right)->value;

	return push(new String(left_value + right_value));
}

Object *VM::build_array(int start_index, int end_index) {
	auto elements = std::vector<Object *>(end_index - start_index, nullptr);

	for (int i = start_index; i < end_index; ++i)
		elements[i - start_index] = m_stack[i];

	return new Array(elements);
}

Object *VM::build_hash(int start_index, int end_index) {
	auto hashtable = new Hash();
	for (int i = start_index; i < end_index; i += 2) {
		auto key = m_stack[i];
		auto value = m_stack[i + 1];
		auto pair = new HashPair{key, value};

		if (!(key->Type() == objecttypes::INTEGER ||
					key->Type() == objecttypes::STRING ||
					key->Type() == objecttypes::BOOLEAN))
			return nullptr;

		HashKey res;
		// we only need to check these types since the previous if expressions
		// guarantees that the object is one of them.
		if (key->Type() == objecttypes::INTEGER)
			res = ((Integer *)key)->hash_key();
		else if (key->Type() == objecttypes::STRING)
			res = ((String *)key)->hash_key();
		else if (key->Type() == objecttypes::BOOLEAN)
			res = ((Boolean *)key)->hash_key();
		hashtable->pairs[res.value] = pair;
	}

	return hashtable;
}

int VM::execute_index_expression(Object *left, Object *index) {
	if (left->Type() == objecttypes::ARRAY_OBJ &&
			index->Type() == objecttypes::INTEGER)
		return execute_array_index(left, index);
	else if (left->Type() == objecttypes::HASH)
		return execute_hash_index(left, index);

	// the index operator is not supported for this type
	return -1;
}

int VM::execute_array_index(Object *arr, Object *index) {
	auto array = dynamic_cast<Array *>(arr);
	if (array == nullptr)
		return -1;

	auto idx = ((Integer *)index)->value;
	auto max = (int)array->elements.size() - 1;

	if (idx < 0 || idx > max) {
		return push(object_constant::null);
	}

	return push(array->elements[idx]);
}

int VM::execute_hash_index(Object *hash, Object *index) {
	auto hashobj = dynamic_cast<Hash *>(hash);
	if (hashobj == nullptr)
		return -1;

	if (!(index->Type() == objecttypes::INTEGER ||
				index->Type() == objecttypes::STRING ||
				index->Type() == objecttypes::BOOLEAN))
		return -1;

	HashKey res;
	// we only need to check these types since the previous if expressions
	// guarantees that the object is one of them.
	if (index->Type() == objecttypes::INTEGER)
		res = ((Integer *)index)->hash_key();
	else if (index->Type() == objecttypes::STRING)
		res = ((String *)index)->hash_key();
	else if (index->Type() == objecttypes::BOOLEAN)
		res = ((Boolean *)index)->hash_key();

	if (hashobj->pairs.count(res.value) == 0)
		return push(object_constant::null);

	return push(hashobj->pairs[res.value]->value);
}

int VM::call_function(Object *func, int num_args) {
	const auto fn = dynamic_cast<CompiledFunction *>(func);
	if (fn == nullptr)
		return -1;

	if (num_args != fn->m_num_parameters)
		return -1;

	auto frame = std::make_unique<Frame>(fn->m_instructions, m_sp - num_args);
	auto new_stack_ptr = frame->base_pointer_ + fn->m_num_locals;
	push_frame(std::move(frame));
	m_sp = new_stack_ptr;

	return 0;
}

int VM::execute_call(int num_args) {
	const auto callee = m_stack[m_sp-1-num_args];

	if (callee->Type() == objecttypes::COMPILED_FUNCTION_OBJ)
		return call_function(callee, num_args);
	else if (callee->Type() == objecttypes::BUILTIN)
		return call_builtin(callee, num_args);

	// calling a non-function.
	return -1;
}

int VM::call_builtin(Object *builtin, int num_args) {
	const auto builtin_func = dynamic_cast<Builtin*>(builtin);
	if (builtin_func == nullptr)
		return -1;

	auto args = std::vector<Object*>(m_stack.begin()+m_sp-num_args, m_stack.begin()+m_sp);
	auto res = (builtin_func->func(args));

	if (res != nullptr) {
		push(res);
	} else {
		push(object_constant::null);
	}

	return 0;
}
