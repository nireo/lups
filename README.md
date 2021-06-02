# lups

A simple interpreted programming language built using C++. The parser is an Operator-precedence parser and the variables have dynamic types.

## Example

```
// A simple recursive fibonacci implementation
let fib = func(n) {
	if (n <= 1) {
		return 0;
	}

	return fib(n-1)+fib(n-2);
};

// print the tenth element in the fibonacci sequence
print(fib(10))

let names_ages = {"bob": 20, "alice": 21};
print("Bob is " + names_ages["bob"] + " years old.");
print("Alice is ", names_ages["alice"], " years old.");
```

## The compiler (WIP)

An already finished version of the language where all the evaluation is done just by interpreting the source code and going from that. The Interpreter resides in the `eval.h | eval.cpp` files. Since it performs quite poorly I've decided to start building a virtual machine and a compiler to improve performance.

## Tests

All of the tests reside in the `tests.cpp` file and can be run by first:
```
# Setup cmake
cmake -S . -B build

# Run script
./run_tests.sh
```

A small todo is for me to seperate the tests into seperated files, but that isn't that important.
