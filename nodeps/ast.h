#ifndef LUPS_AST_H
#define LUPS_AST_H

#include <memory>
#include <string>
#include <vector>
#include "token.h"

class Node {
public:
	virtual ~Node(){};
	virtual std::string literal() = 0;
	virtual std::string type();
	virtual std::string string() = 0;
};

class Statement : public Node {
public:
	virtual void statement_node() = 0;
	virtual std::string literal() = 0;
	virtual std::string type() = 0;
	virtual std::string string() = 0;
};

class Expression : public Node {
public:
	virtual void expression_node() = 0;
	virtual std::string type() = 0;
	virtual std::string string() = 0;
	virtual std::string literal() = 0;
};

class Program {
public:
	std::string literal();
	std::string string();
	std::string type() { return "program"; }

	std::vector<std::unique_ptr<Statement>> m_statements;
};


class Identifier : public Expression {
 public:
	void expression_node() {};
	std::string literal() { return tok.literal; }
	std::string string() { return value; }
	std::string type() { return "Identifier"; }

	Token tok;
	std::string value;
};

class LetStatement : public Statement {
public:
	void statement_node() {}
	std::string literal() { return tok.literal; }
	std::string string() {
		return "";
	}
	std::string type() { return "LetStatement"; }
	std::unique_ptr<Identifier> name;
	std::unique_ptr<Expression> val;

	Token tok;
};

#endif
