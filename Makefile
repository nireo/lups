all:
	g++ benchmark.cpp lexer.cpp eval.cpp parser.cpp ast.cpp compiler.cpp vm.cpp code.cpp builtins.cpp -o bench -g

main:
	g++ main.cpp lexer.cpp eval.cpp parser.cpp ast.cpp compiler.cpp vm.cpp code.cpp builtins.cpp -o lups -g -std=c++17 -O2
