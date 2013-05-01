test:
	@(cd tests; g++ --std=c++0x tests.cpp; ./a.out; rm a.out)
