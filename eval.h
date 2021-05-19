#ifndef LUPS_EVAL_H
#define LUPS_EVAL_H

#include "object.h"
#include <memory>
#include "ast.h"

namespace eval {
	std::unique_ptr<Object> Eval(std::unique_ptr<Node> node);
}

#endif
