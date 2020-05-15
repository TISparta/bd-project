CXX = g++
CXXFLAGS = -std=c++14 -O2 -lgtest -lgtest_main -pthread

test: FORCE
	$(CXX) -o bplustree-test $(CXXFLAGS) bplustree-test.cpp

FORCE: ;

clean:
	rm btree-test
