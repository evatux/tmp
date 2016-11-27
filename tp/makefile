CXX = g++ -std=c++11 -g

headers = $(wildcard *.hpp)

a.out: conv.o
	$(CXX) $< -o $@ -lpthread

%.o: %.cpp $(headers)
	$(CXX) $< -c -o $@

clean: ; -rm -f *.o a.out
