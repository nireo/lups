#include "ast.h"

std::string Program::literal() {
	if (m_statements.size() > 0) {
		return m_statements[0].get()->literal();
	}
	return "";
}
