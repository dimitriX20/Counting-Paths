CC := g++
CFLAGS := -std=c++17 -O3
INCLUDES := -I./homlib/src -I/home/dimitri/.local/include 
SRCDIR := ./homlib/src
OBJDIR := ./homlib/obj

SOURCES := $(wildcard $(SRCDIR)/*.cc)
OBJECTS := $(patsubst $(SRCDIR)/%.cc, $(OBJDIR)/%.o, $(SOURCES))

.PHONY: all clean

all: myprogram

$(OBJDIR)/%.o: $(SRCDIR)/%.cc | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

myprogram: $(OBJDIR)/homlib.o $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) myprogram
