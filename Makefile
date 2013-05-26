CXX_FLAGS = --std=c++0x -Werror

all:
	@(echo "Noting to buid")

test:
	@(cd tests;$(CXX) $(CXX_FLAGS) tests.cpp; ./a.out || [ $$? -eq 0 ])

test-vg:
	@(cd tests;$(CXX) $(CXX_FLAGS) tests.cpp; valgrind --leak-check=full --error-exitcode=1 ./a.out || [ $$? -eq 0 ])
