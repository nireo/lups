#include "builtins.h"
#include <iostream>
#include "eval.h"

Object *builtins::len(std::vector<Object *> &objs) {
	if (objs.size() != 2) {
		return new Error("wrong number of arguments. want 2");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto array = (Array *)objs[0];
	auto length = array->elements.size();
	std::vector<Object *> new_elements = array->elements;
	new_elements.push_back(objs[1]);

	return new Array(new_elements);
}

Object *builtins::println(std::vector<Object *> &objs) {
	for (const auto& arg : objs) {
		std::cout << arg->Inspect() << ' ';
	}
	std::cout << '\n';

	return nullptr;
}

Object *builtins::array_first(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto elements = ((Array *)objs[0])->elements;
	if (elements.size() > 0) {
		return elements[0];
	}

	return object_constant::null;
}

Object *builtins::array_last(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto elements = ((Array *)objs[0])->elements;
	auto size = elements.size();
	if (size > 0) {
		return elements[size - 1];
	}

	return object_constant::null;
}

Object *builtins::array_tail(std::vector<Object *> &objs) {
	if (objs.size() != 1) {
		return new Error("wrong number of arguments. want 1");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto elements = ((Array *)objs[0])->elements;
	auto size = elements.size();
	if (size > 0) {
		std::vector<Object *> tailed_elements(elements.begin() + 1, elements.end());
		return new Array(tailed_elements);
	}

	return object_constant::null;
}

Object *builtins::array_push(std::vector<Object *> &objs) {
	if (objs.size() != 2) {
		return new Error("wrong number of arguments. want 2");
	}

	if (objs[0]->Type() != objecttypes::ARRAY_OBJ) {
		return new Error("the argument must be of type array, got: " +
										 objs[0]->Type());
	}

	auto array = (Array *)objs[0];
	auto length = array->elements.size();
	std::vector<Object *> new_elements = array->elements;
	new_elements.push_back(objs[1]);

	return new Array(new_elements);
}
