#include "builtins.h"
#include "eval.h"
#include <iostream>

const std::array<std::pair<std::string, Builtin *>, 6>
		builtin_functions::functions = {
				std::make_pair("len", new Builtin(*builtin_functions::len)),
				std::make_pair("println", new Builtin(*builtin_functions::println)),
				std::make_pair("first", new Builtin(*builtin_functions::array_first)),
				std::make_pair("last", new Builtin(*builtin_functions::array_last)),
				std::make_pair("tail", new Builtin(*builtin_functions::array_tail)),
				std::make_pair("push", new Builtin(*builtin_functions::array_push)),
};

Object *builtin_functions::len(std::vector<Object *> &objs) {
	if (objs.size() != 2) {
		return new Error("wrong number of arguments. want 2");
	}

	if (objs[0]->Type() != ObjType::Array && objs[0]->Type() != ObjType::String) {
		return new Error("the argument to 'len' must be of type array or string");
	}

	if (objs[0]->Type() == ObjType::Array) {
		return new Integer(((Array *)objs[0])->elements.size());
	} else if (objs[0]->Type() == ObjType::String) {
		return new Integer(((String *)objs[0])->value.size());
	}

	// shouldn't be possible to reach this.
	return object_constant::null;
}

Object *builtin_functions::println(std::vector<Object *> &objs) {
	for (const auto &arg : objs) {
		std::cout << arg->Inspect() << ' ';
	}
	std::cout << '\n';

	return nullptr;
}

Object *builtin_functions::array_first(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != ObjType::Array) {
		return new Error("the argument must be of type array");
	}

	auto elements = ((Array *)objs[0])->elements;
	if (elements.size() > 0) {
		return elements[0];
	}

	return object_constant::null;
}

Object *builtin_functions::array_last(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != ObjType::Array) {
		return new Error("the argument must be of type array");
	}

	auto elements = ((Array *)objs[0])->elements;
	auto size = elements.size();
	if (size > 0) {
		return elements[size - 1];
	}

	return object_constant::null;
}

Object *builtin_functions::array_tail(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != ObjType::Array) {
		return new Error("the argument must be of type array");
	}

	auto elements = ((Array *)objs[0])->elements;
	auto size = elements.size();
	if (size > 0) {
		std::vector<Object *> tailed_elements(elements.begin() + 1, elements.end());
		return new Array(tailed_elements);
	}

	return object_constant::null;
}

Object *builtin_functions::array_push(std::vector<Object *> &objs) {
	if (objs.size() != 2) {
		return new Error("wrong number of arguments. want 2");
	}

	if (objs[0]->Type() != ObjType::Array) {
		return new Error("the argument must be of type array");
	}

	auto array = (Array *)objs[0];
	auto length = array->elements.size();
	std::vector<Object *> new_elements = array->elements;
	new_elements.push_back(objs[1]);

	return new Array(new_elements);
}
