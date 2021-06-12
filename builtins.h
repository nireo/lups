#ifndef LUPS_BUILTIN_H
#define LUPS_BUILTIN_H

#include "object.h"
#include <map>
#include <vector>

namespace builtin_functions {
Object *array_push(std::vector<Object *> &objs);
Object *array_tail(std::vector<Object *> &objs);
Object *array_last(std::vector<Object *> &objs);
Object *array_first(std::vector<Object *> &objs);
Object *println(std::vector<Object *> &objs);
Object *len(std::vector<Object *> &objs);

// we store them in an array such that the function indices are predictable.
extern const std::array<std::pair<std::string, Builtin *>, 6> functions;
} // namespace builtin_functions

#endif
