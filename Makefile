CXX = g++
CXX_FLAGS = --std=c++0x

test:
	@(cd tests; $(CXX) $(CXX_FLAGS) tests.cpp; ./a.out; rm a.out)
