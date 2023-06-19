CC := g++
CFLAGS := -std=c++17 -O3
INCLUDES := -I./homlib/src -I/home/dimitri/.local/include -I./nauty2_8_6
SRCDIR := ./homlib/src
OBJDIR := ./homlib/obj
LIBDIR := ./nauty2_8_6
LIBS := $(LIBDIR)/nauty.a

SOURCES := $(wildcard $(SRCDIR)/*.cc)
OBJECTS := $(patsubst $(SRCDIR)/%.cc, $(OBJDIR)/%.o, $(SOURCES))

.PHONY: all clean nauty

all: nauty myprogram

$(OBJDIR)/%.o: $(SRCDIR)/%.cc | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

nauty:
	$(MAKE) -C $(LIBDIR)

myprogram: $(OBJDIR)/homlib.o $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) myprogram
	$(MAKE) -C $(LIBDIR) clean
