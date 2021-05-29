all:
	g++ repl.cpp lexer.cpp eval.cpp parser.cpp ast.cpp compiler.cpp vm.cpp code.cpp -o lups -g
