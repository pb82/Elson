CXX_FLAGS = --std=c++0x

all:
	@(echo "Noting to buid")

test:
	@(cd tests;$(CXX) $(CXX_FLAGS) tests.cpp;./a.out || [ $$? -eq 0 ])
