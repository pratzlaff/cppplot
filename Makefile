PGPLOT = /usr/local/pgplot
CXX = g++
CXXFLAGS = -Wall -W -pedantic -gstabs+ -I$(PGPLOT)
LDFLAGS = -Wl,-R$(PGPLOT) -L$(PGPLOT) -lcpgplot -lpgplot -ldl
LDFLAGS = -lcpgplot
#LDFLAGS = -L/usr/local/pgplot -lcpgplot -lpgplot -ldl#$(shell perl -MExtUtils::F77 -e 42 | grep Runtime | perl -ple 's/^Runtime:\s+//')
#LDFLAGS = -L/usr/local/pgplot -Wl,-Bstatic -lcpgplot -lpgplot -Wl,-Bdynamic -L/usr/X11R6/lib -lX11 -lpng -lz $(shell perl -MExtUtils::F77 -e 42 | grep Runtime | perl -ple 's/^Runtime:\s+//')
#LDFLAGS = -L/usr/local/pgplot -lcpgplot -lpgplot -L/usr/X11R6/lib -lX11 -lpng -lz $(shell perl -MExtUtils::F77 -e 42 | grep Runtime | perl -ple 's/^Runtime:\s+//')

DEMOS = demo1 demo2

all: $(DEMOS)

% : %.cc pgplot.hh
	$(CXX) $(CXXFLAGS) -o $@  $< $(LDFLAGS)

clean:
	rm -f *.o *~ $(DEMOS)
