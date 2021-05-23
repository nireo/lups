#ifndef LUPS_AST_H
#define LUPS_AST_H

#include "token.h"
#include <memory>
#include <string>
#include <vector>

class Node {
public:
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
	void statementNode() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "ReturnStatement"; }

	Token token;
	std::unique_ptr<Expression> return_value;
};

class ExpressionStatement : public Statement {
public:
	void statementNode() {}
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "ExpressionStatement"; }

	Token token;
	std::unique_ptr<Expression> expression;
};

class IntegerLiteral : public Expression {
public:
	std::string TokenLiteral() { return token.literal; }
	std::string String() { return token.literal; }
	std::string Type() { return "IntegerLiteral"; }

	Token token;
	int value;
};

class PrefixExpression : public Expression {
public:
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "PrefixExpression"; }

	Token token;

	// the operator
	std::string opr;
	std::unique_ptr<Expression> right;
};

class InfixExpression : public Expression {
public:
	std::string TokenLiteral() { return token.literal; }
	std::string String();
	std::string Type() { return "InfixExpression"; }

	Token token;
	std::string opr;
	std::unique_ptr<Expression> right;
	std::unique_ptr<Expression> left;
};

class BooleanExpression : public Expression {
public:
	std::string String() { return token.literal; }
	std::string Type() { return "BooleanExpression"; }
	std::string TokenLiteral() { return token.literal; }

	Token token;
	bool value;
};


class BlockStatement : public Statement {
public:
	void statementNode() {}
	std::string String() { return token.literal; }
	std::string Type() { return "BlockExpression"; }
	std::string TokenLiteral() { return token.literal; }

	Token token;
	std::vector<std::unique_ptr<Statement>> statements;
};

class IfExpression : public Expression {
public:
	std::string String() { return token.literal; }
	std::string Type() { return "IfExpression"; }
	std::string TokenLiteral() { return token.literal; }

	Token token;
	std::unique_ptr<Expression> cond;
	std::unique_ptr<BlockStatement> after;
	std::unique_ptr<BlockStatement> other;
};

class FunctionLiteral : public Expression {
public:
	std::string String() { return token.literal; }
	std::string Type() { return "FunctionLiteral"; }
	std::string TokenLiteral() { return token.literal; }

	Token token;
	std::vector<std::unique_ptr<Identifier>> params;
	std::unique_ptr<BlockStatement> body;
};

class CallExpression : public Expression {
public:
	std::string String();
	std::string TokenLiteral() { return token.literal; }
	std::string Type() { return "CallExpression"; }

	Token token;
	std::vector<std::unique_ptr<Expression>> arguments;
	std::unique_ptr<Expression> func;
};

class StringLiteral : public Expression {
public:
	std::string String() { return value; }
	std::string TokenLiteral() { return token.literal; }
	std::string Type() { return "StringLiteral"; }

	Token token;
	std::string value;
};

class ArrayLiteral : public Expression {
public:
	std::string String();
	std::string TokenLiteral() { return token.literal; }
	std::string Type() { return "ArrayLiteral"; }

	std::vector<std::unique_ptr<Expression>> elements;
	Token token;
};

#endif
