#ifndef LUPS_AST_H
#define LUPS_AST_H

#include <memory>
#include <string>
#include <vector>
#include "token.h"

class Node {
 public:
	virtual ~Node() { };
	virtual std::string TokenLiteral() = 0;
	virtual std::string String() = 0;
	virtual std::string Type() = 0;
};

// here I haven't found any good solution to nesting interface.
class Statement : public Node {
 public:
	virtual void statementNode() = 0;
	virtual std::string TokenLiteral() = 0;
	virtual std::string String() = 0;
	virtual std::string Type() = 0;
};

class Expression : public Node {
 public:
	virtual std::string TokenLiteral() = 0;
	virtual std::string String() = 0;
	virtual std::string Type() = 0;
};

class Program : public Node {
 public:
	~Program() {
	}
	std::string TokenLiteral();
	std::string String();
	std::string Type() { return "Program"; }

	std::vector<std::unique_ptr<Statement>> statements;
};

class Identifier : public Expression {
 public:
	std::string TokenLiteral() { return token.literal; }
	std::string String() { return value; }
	std::string Type() { return "Identifier"; }

	Token token;
	std::string value;
};

class LetStatement : public Statement {
 public:
	~LetStatement() {
	}
	void statementNode() { }
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "LetStatement"; }

	Token token;
	std::unique_ptr<Identifier> name;
	std::unique_ptr<Expression> value;
};



#endif
