CXXFLAGS = -Wall -W -pedantic --std=c++14
LDFLAGS = -lcpgplot

DEMOS = demo1 demo2

all: $(DEMOS)

% : %.cc pgplot.hh
	$(CXX) $(CXXFLAGS) -o $@  $< $(LDFLAGS)

clean:
	rm -f *.o *~ $(DEMOS)
