# lups

A simple interpreted programming language built using C++. The parser is an Operator-precedence parser and the variables have dynamic types.

## Example

```
// A sketch of the language I had in mind.
let fib = func(n) {
	if (n <= 1) {
		return 0;
	}

	return fib(n-1)+fib(n-2);
};
```
