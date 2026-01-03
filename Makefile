CXXFLAGS = -Wextra -pedantic --std=c++14
LDFLAGS = -l:libcpgplot.so.0

DEMOS = demo1 demo2

all: $(DEMOS)

% : %.cc pgplot.hh
	$(CXX) $(CXXFLAGS) -o $@  $< $(LDFLAGS)

clean:
	rm -f *.o *~ $(DEMOS)
