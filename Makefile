all:
	g++ repl.cpp lexer.cpp eval.cpp parser.cpp ast.cpp -o lups -g
