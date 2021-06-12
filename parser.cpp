#include "parser.h"
#include "ast.h"
#include "token.h"
#include <algorithm>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

template <typename T> T FromString(const std::string &str) {
	std::istringstream ss(str);
	T ret;
	ss >> ret;
	return ret;
}

using std::unique_ptr;

unique_ptr<Program> Parser::parse_program() {
	auto program = std::make_unique<Program>();
	program->statements = std::vector<unique_ptr<Statement>>();

	while (current_.type != tokentypes::EOFF) {
		auto stmt = parse_statement();
		if (stmt != nullptr) {
			program->statements.push_back(std::move(stmt));
		}
		next_token();
	}

	return program;
}

Parser::Parser(unique_ptr<Lexer> lx) {
	lx_ = std::move(lx);

	m_prefix_parse_fns = std::unordered_map<TokenType, PrefixParseFn>();

	// TODO: place straight into map
	add_prefix_parse(tokentypes::IDENT, &Parser::parse_identifier);
	add_prefix_parse(tokentypes::INT, &Parser::parse_integer_literal);
	add_prefix_parse(tokentypes::BANG, &Parser::parse_prefix_expression);
	add_prefix_parse(tokentypes::MINUS, &Parser::parse_prefix_expression);
	add_prefix_parse(tokentypes::TRUE, &Parser::parse_boolean);
	add_prefix_parse(tokentypes::FALSE, &Parser::parse_boolean);
	add_prefix_parse(tokentypes::LPAREN, &Parser::parse_grouped_expression);
	add_prefix_parse(tokentypes::IF, &Parser::parse_if_expression);
	add_prefix_parse(tokentypes::FUNCTION, &Parser::parse_function_literal);
	add_prefix_parse(tokentypes::STRING, &Parser::parse_string_literal);
	add_prefix_parse(tokentypes::LBRACKET, &Parser::parse_array_literal);
	add_prefix_parse(tokentypes::LBRACE, &Parser::parse_hash_literal);

	m_infix_parse_fns = std::unordered_map<TokenType, InfixParseFn>();
	add_infix_parse(tokentypes::PLUS, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::MINUS, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::SLASH, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::ASTERISK, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::EQ, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::NEQ, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::LT, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::GT, &Parser::parse_infix_expression);
	add_infix_parse(tokentypes::LPAREN, &Parser::parse_call_expression);
	add_infix_parse(tokentypes::LBRACKET, &Parser::parse_index_expression);

	next_token();
	next_token();
}

void Parser::next_token() {
	current_ = peek_;
	peek_ = lx_->next_token();
}

unique_ptr<Statement> Parser::parse_statement() {
	if (current_.type == tokentypes::LET) {
		return parse_let_statement();
	} else if (current_.type == tokentypes::RETURN) {
		return parse_return_statement();
	} else {
		return parse_expression_statement();
	}
}

bool Parser::current_token_is(TokenType tt) { return current_.type == tt; }

bool Parser::peek_token_is(TokenType tt) { return peek_.type == tt; }

bool Parser::expect_peek(TokenType tt) {
	if (peek_token_is(tt)) {
		next_token();
		return true;
	}

	peek_error(tt);
	return false;
}

unique_ptr<Statement> Parser::parse_let_statement() {
	auto letstmt = std::make_unique<LetStatement>();
	letstmt->token = current_;

	if (!expect_peek(tokentypes::IDENT)) {
		return nullptr;
	}

	auto ident = std::make_unique<Identifier>();
	ident->token = current_;
	ident->value = current_.literal;
	letstmt->name = std::move(ident);

	if (!expect_peek(tokentypes::ASSIGN)) {
		return nullptr;
	}

	next_token();

	letstmt->value = parse_expression(LOWEST);

	if (peek_token_is(tokentypes::SEMICOLON))
		next_token();

	return letstmt;
}

unique_ptr<Statement> Parser::parse_return_statement() {
	auto returnstmt = std::make_unique<ReturnStatement>();
	returnstmt->token = current_;

	next_token();
	returnstmt->return_value = parse_expression(LOWEST);

	if (peek_token_is(tokentypes::SEMICOLON))
		next_token();

	return returnstmt;
}

unique_ptr<Statement> Parser::parse_expression_statement() {
	auto stmt = std::make_unique<ExpressionStatement>();
	stmt->token = current_;

	stmt->expression = parse_expression(LOWEST);
	if (peek_token_is(tokentypes::SEMICOLON)) {
		next_token();
	}

	return stmt;
}

unique_ptr<Expression> Parser::parse_expression(Precedence prec) {
	if (m_prefix_parse_fns.find(current_.type) == m_prefix_parse_fns.end()) {
		return nullptr;
	}

	auto fn = m_prefix_parse_fns[current_.type];
	auto left = (this->*fn)();

	while (!peek_token_is(tokentypes::SEMICOLON) && prec < peek_precedence()) {
		auto infix = m_infix_parse_fns[peek_.type];
		if (infix == nullptr)
			return left;
		next_token();
		left = (this->*infix)(std::move(left));
	}

	return left;
}

unique_ptr<Expression> Parser::parse_identifier() {
	auto identifier = std::make_unique<Identifier>();
	identifier->token = current_;
	identifier->value = current_.literal;

	return identifier;
}

unique_ptr<Expression> Parser::parse_integer_literal() {
	auto lit = std::make_unique<IntegerLiteral>();
	lit->token = current_;

	try {
		int res = std::stoi(current_.literal);
		lit->value = res;
	} catch (std::invalid_argument e) {
		errors_.push_back("could not parse integer");
		return nullptr;
	}

	return lit;
}

unique_ptr<Expression> Parser::parse_prefix_expression() {
	auto exp = std::make_unique<PrefixExpression>();
	exp->token = current_;
	exp->opr = current_.literal;

	next_token();
	exp->right = parse_expression(PREFIX);

	return exp;
}

Precedence Parser::peek_precedence() {
	if (precedences.find(peek_.type) != precedences.end())
		return precedences.at(peek_.type);
	return LOWEST;
}

Precedence Parser::current_precedence() {
	if (precedences.find(current_.type) != precedences.end())
		return precedences.at(current_.type);
	return LOWEST;
}

unique_ptr<Expression>
Parser::parse_infix_expression(unique_ptr<Expression> left) {
	auto exp = std::make_unique<InfixExpression>();
	exp->token = current_;
	exp->opr = current_.literal;
	exp->left = std::move(left);

	auto prec = current_precedence();
	next_token();
	exp->right = std::move(parse_expression(prec));

	return exp;
}

unique_ptr<Expression> Parser::parse_grouped_expression() {
	next_token();
	auto exp = parse_expression(LOWEST);
	if (!expect_peek(tokentypes::RPAREN))
		return nullptr;

	return exp;
}

unique_ptr<Expression> Parser::parse_boolean() {
	auto exp = std::make_unique<BooleanExpression>();
	exp->token = current_;
	exp->value = current_token_is(tokentypes::TRUE);

	return exp;
}

unique_ptr<Expression> Parser::parse_if_expression() {
	auto exp = std::make_unique<IfExpression>();
	exp->token = current_;

	if (!expect_peek(tokentypes::LPAREN))
		return nullptr;

	next_token();
	exp->cond = parse_expression(LOWEST);

	if (!expect_peek(tokentypes::RPAREN))
		return nullptr;

	if (!expect_peek(tokentypes::LBRACE))
		return nullptr;

	exp->after = parse_block_statement();

	if (peek_token_is(tokentypes::ELSE)) {
		next_token();

		if (!expect_peek(tokentypes::LBRACE)) {
			return nullptr;
		}

		exp->other = parse_block_statement();
	}

	return exp;
}

unique_ptr<BlockStatement> Parser::parse_block_statement() {
	auto block = std::make_unique<BlockStatement>();
	block->token = current_;
	block->statements = std::vector<unique_ptr<Statement>>();

	next_token();

	while (!current_token_is(tokentypes::RBRACE) &&
				 !current_token_is(tokentypes::EOFF)) {
		auto stmt = parse_statement();
		if (stmt != nullptr) {
			block->statements.push_back(std::move(stmt));
		}
		next_token();
	}

	return block;
}

unique_ptr<Expression> Parser::parse_function_literal() {
	auto lit = std::make_unique<FunctionLiteral>();
	lit->token = current_;

	if (!expect_peek(tokentypes::LPAREN))
		return nullptr;

	lit->params = parse_function_params();

	if (!expect_peek(tokentypes::LBRACE))
		return nullptr;

	lit->body = std::move(parse_block_statement());

	return lit;
}

std::vector<unique_ptr<Identifier>> Parser::parse_function_params() {
	std::vector<unique_ptr<Identifier>> params;
	if (peek_token_is(tokentypes::LPAREN)) {
		next_token();
		return params;
	}

	next_token();

	auto ident = std::make_unique<Identifier>();
	ident->token = current_;
	ident->value = current_.literal;
	params.push_back(std::move(ident));

	while (peek_token_is(tokentypes::COMMA)) {
		next_token();
		next_token();

		auto ident = std::make_unique<Identifier>();
		ident->token = current_;
		ident->value = current_.literal;
		params.push_back(std::move(ident));
	}

	if (!expect_peek(tokentypes::RPAREN))
		return std::vector<unique_ptr<Identifier>>();

	return params;
}

unique_ptr<Expression>
Parser::parse_call_expression(unique_ptr<Expression> func) {
	auto exp = std::make_unique<CallExpression>();
	exp->token = current_;
	exp->func = std::move(func);
	exp->arguments = std::move(parse_expression_list(tokentypes::RPAREN));

	return exp;
}

unique_ptr<Expression> Parser::parse_string_literal() {
	auto strlit = std::make_unique<StringLiteral>();
	strlit->token = current_;
	strlit->value = current_.literal;

	return strlit;
}

unique_ptr<Expression> Parser::parse_array_literal() {
	auto arr = std::make_unique<ArrayLiteral>();
	arr->token = current_;
	arr->elements = std::move(parse_expression_list(tokentypes::RBRACKET));

	return arr;
}

std::vector<unique_ptr<Expression>>
Parser::parse_expression_list(TokenType end) {
	std::vector<unique_ptr<Expression>> expressions;
	if (peek_token_is(end)) {
		next_token();
		return expressions;
	}

	next_token();
	expressions.push_back(std::move(parse_expression(LOWEST)));
	while (peek_token_is(tokentypes::COMMA)) {
		next_token();
		next_token();

		expressions.push_back(std::move(parse_expression(LOWEST)));
	}

	if (!expect_peek(end))
		return std::vector<unique_ptr<Expression>>();

	return expressions;
}

std::vector<unique_ptr<Expression>> Parser::parse_call_arguments() {
	std::vector<unique_ptr<Expression>> args;

	if (peek_token_is(tokentypes::RPAREN)) {
		next_token();
		return args;
	}

	next_token();
	args.push_back(std::move(parse_expression(LOWEST)));

	while (peek_token_is(tokentypes::COMMA)) {
		next_token();
		next_token();

		args.push_back(std::move(parse_expression(LOWEST)));
	}

	if (!expect_peek(tokentypes::RPAREN))
		return std::vector<unique_ptr<Expression>>();

	return args;
}

std::unique_ptr<Expression>
Parser::parse_index_expression(std::unique_ptr<Expression> left) {
	auto exp = std::make_unique<IndexExpression>();
	exp->token = current_;
	exp->left = std::move(left);

	next_token();
	exp->index = parse_expression(LOWEST);
	if (!expect_peek(tokentypes::RBRACKET))
		return nullptr;

	return exp;
}

unique_ptr<Expression> Parser::parse_hash_literal() {
	auto hash = std::make_unique<HashLiteral>();
	std::vector<std::pair<unique_ptr<Expression>, unique_ptr<Expression>>> pairs;

	while (!peek_token_is(tokentypes::RBRACE)) {
		next_token();
		auto key = parse_expression(LOWEST);
		if (!expect_peek(tokentypes::COLON)) {
			return nullptr;
		}

		next_token();
		auto value = parse_expression(LOWEST);
		pairs.push_back(std::make_pair(std::move(key), std::move(value)));

		if (!peek_token_is(tokentypes::RBRACE) && !expect_peek(tokentypes::COMMA))
			return nullptr;
	}

	if (!expect_peek(tokentypes::RBRACE))
		return nullptr;

	hash->pairs = std::move(pairs);

	return hash;
}

std::vector<std::string> Parser::errors() const { return errors_; }

void Parser::peek_error(TokenType tt) {
	std::string err =
			"expected next token to be " + tt + " got " + peek_.type + " instead";
	errors_.push_back(err);
}

void Parser::add_prefix_parse(TokenType tt, PrefixParseFn fn) {
	m_prefix_parse_fns[tt] = fn;
}

void Parser::add_infix_parse(TokenType tt, InfixParseFn fn) {
	m_infix_parse_fns[tt] = fn;
}
