CXX = g++
CXXFLAGS = -std=c++17 -std=gnu++17 -Wall -Wextra

OBJS = icmpchecksum.o check_if_valid_input.o receive.o send.o print_output.o traceroute.o

all: traceroute

traceroute: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o

distclean: clean
	rm -f traceroute
