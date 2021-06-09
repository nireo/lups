#ifndef LUPS_COMPILER_H
#define LUPS_COMPILER_H

#include "object.h"
#include <map>
#include <vector>

namespace builtins {
Object *array_push(std::vector<Object *> &objs);
Object *array_tail(std::vector<Object *> &objs);
Object *array_last(std::vector<Object *> &objs);
Object *array_first(std::vector<Object *> &objs);
Object *println(std::vector<Object *> &objs);
Object *len(std::vector<Object *> &objs);

// we store them in an array such that the function indices are predictable.
static const std::array<std::pair<std::string, Builtin *>, 6> functions{
		std::make_pair("len", new Builtin(*builtins::len)),
		std::make_pair("println", new Builtin(*builtins::println)),
		std::make_pair("first", new Builtin(*builtins::array_first)),
		std::make_pair("last", new Builtin(*builtins::array_last)),
		std::make_pair("tail", new Builtin(*builtins::array_tail)),
		std::make_pair("push", new Builtin(*builtins::array_push)),
};
} // namespace builtins

#endif
