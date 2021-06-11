all:
	g++ benchmark.cpp lexer.cpp eval.cpp parser.cpp ast.cpp compiler.cpp vm.cpp code.cpp builtins.cpp -o bench -g
