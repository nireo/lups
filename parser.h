#ifndef LUPS_PARSER_H
#define LUPS_PARSER_H

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include <memory>
#include <unordered_map>

class Parser;
typedef std::unique_ptr<Expression> (Parser::*PrefixParseFn)();
typedef std::unique_ptr<Expression> (Parser::*InfixParseFn)(
		std::unique_ptr<Expression>);


// TODO: make scoped enum
enum Precedence {
	LOWEST,
	EQUALS,
	LESSGREATER,
	SUM,
	PRODUCT,
	PREFIX,
	CALL,
	INDEX,
};

const std::unordered_map<TokenType, Precedence> precedences = {
		{tokentypes::EQ, EQUALS},
		{tokentypes::NEQ, EQUALS},
		{tokentypes::LT, LESSGREATER},
		{tokentypes::GT, LESSGREATER},
		{tokentypes::PLUS, SUM},
		{tokentypes::MINUS, SUM},
		{tokentypes::SLASH, PRODUCT},
		{tokentypes::ASTERISK, PRODUCT},
		{tokentypes::LPAREN, CALL},
		{tokentypes::LBRACKET, INDEX},
};

class Parser {
public:
	Parser(std::unique_ptr<Lexer> lx);
	std::unique_ptr<Program> parse_program();
	std::vector<std::string> errors() const;
private:
	std::unique_ptr<Lexer> lx_;
	Token current_;
	Token peek_;

	std::unordered_map<TokenType, PrefixParseFn> m_prefix_parse_fns;
	std::unordered_map<TokenType, InfixParseFn> m_infix_parse_fns;
	void add_prefix_parse(TokenType tt, PrefixParseFn fn);
	void add_infix_parse(TokenType tt, InfixParseFn fn);

	void next_token();

	std::unique_ptr<Statement> parse_statement();
	std::unique_ptr<Statement> parse_let_statement();
	std::unique_ptr<Statement> parse_return_statement();
	std::unique_ptr<Statement> parse_expression_statement();
	std::unique_ptr<Expression> parse_expression(Precedence prec);
	std::unique_ptr<Expression> parse_identifier();
	std::unique_ptr<Expression> parse_integer_literal();
	std::unique_ptr<Expression> parse_prefix_expression();
	std::unique_ptr<Expression> parse_infix_expression(std::unique_ptr<Expression> left);
	std::unique_ptr<Expression> parse_boolean();
	std::unique_ptr<Expression> parse_grouped_expression();
	std::unique_ptr<Expression> parse_if_expression();
	std::unique_ptr<BlockStatement> parse_block_statement();
	std::unique_ptr<Expression> parse_function_literal();
	std::unique_ptr<Expression> parse_string_literal();
	std::unique_ptr<Expression> parse_call_expression(std::unique_ptr<Expression> func);
	std::unique_ptr<Expression> parse_array_literal();
	std::unique_ptr<Expression> parse_hash_literal();
	std::unique_ptr<Expression> parse_index_expression(std::unique_ptr<Expression> left);
	std::vector<std::unique_ptr<Expression>> parse_expression_list(TokenType end);

	std::vector<std::unique_ptr<Identifier>> parse_function_params();
	std::vector<std::unique_ptr<Expression>> parse_call_arguments();

	Precedence peek_precedence();
	Precedence current_precedence();

	bool expect_peek(TokenType tt);
	bool peek_token_is(TokenType tt);
	bool current_token_is(TokenType tt);
	void peek_error(TokenType tt);

	std::vector<std::string> errors_;
};

#endif
