CXX = clang++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Wpedantic -pthread
LDFLAGS = -lenet

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build
BINDIR = bin

# Source files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

# Executable name
EXECUTABLE = $(BINDIR)/enetboom

# Make sure build directories exist
$(shell mkdir -p $(BUILDDIR) $(BINDIR))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)/* $(BINDIR)/*

.PHONY: all clean
