#ifndef LUPS_AST_H
#define LUPS_AST_H

#include "token.h"
#include <memory>
#include <string>
#include <vector>

class Node {
public:
	virtual ~Node(){};
	virtual std::string TokenLiteral() = 0;
	virtual std::string String() = 0;
	virtual std::string Type() = 0;
};

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
	~Program() {}
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
	~LetStatement() {}
	void statementNode() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "LetStatement"; }

	Token token;
	std::unique_ptr<Identifier> name;
	std::unique_ptr<Expression> value;
};

class ReturnStatement : public Statement {
public:
	~ReturnStatement() {}
	void statementNode() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "ReturnStatement"; }

	Token token;
	std::unique_ptr<Expression> return_value;
};

class ExpressionStatement : public Statement {
public:
	~ExpressionStatement() {}
	void statementNode() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "ExpressionStatement"; }

	Token token;
	std::unique_ptr<Expression> expression;
};

class IntegerLiteral : public Expression {
public:
	~IntegerLiteral() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String() { return token.literal; }
	std::string Type() { return "ExpressionStatement"; }

	Token token;
	int value;
};

class PrefixExpression : public Expression {
public:
	~PrefixExpression() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "PrefixEXpression"; }

	Token token;

	// the operator
	std::string opr;
	std::unique_ptr<Expression> right;
};

#endif
