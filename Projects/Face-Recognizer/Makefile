CXXFLAGS = $(shell pkg-config --cflags opencv4)
LDFLAGS  = $(shell pkg-config --libs opencv4)
PROG     = prog

all: $(PROG)

$(PROG): main.cpp
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(PROG)

.PHONY: all clean
