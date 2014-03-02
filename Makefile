CXX_FLAGS = --std=c++0x -Werror

all:
	@(echo "Nothing to buid")

test:
	@(cd tests;rm -f a.out; $(CXX) $(CXX_FLAGS) tests.cpp; ./a.out || [ $$? -eq 0 ])

test-vg:
	@(cd tests;rm -f a.out; $(CXX) $(CXX_FLAGS) tests.cpp; valgrind --leak-check=full --error-exitcode=1 ./a.out || [ $$? -eq 0 ])
