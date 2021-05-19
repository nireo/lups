#ifndef LUPS_EVAL_H
#define LUPS_EVAL_H

#include "object.h"
#include "ast.h"

namespace eval {
	Object* Eval(Node* node);
	Object* eval_statements(Node* program);
}

#endif
