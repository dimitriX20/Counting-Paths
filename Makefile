CC := g++
CFLAGS := -std=c++17 -O3 -pthread -fsanitize=address
INCLUDES := -I./homlib/src -I/home/dimitri/.local/include -I./nauty2_8_6
SRCDIR := ./homlib/src
OBJDIR := ./homlib/obj
LIBDIR := ./nauty2_8_6
LIBS := $(LIBDIR)/nauty.a

SOURCES := $(wildcard $(SRCDIR)/*.cc)
OBJECTS := $(patsubst $(SRCDIR)/%.cc, $(OBJDIR)/%.o, $(SOURCES))

.PHONY: all clean nauty

all: nauty myprogram run_script

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

run_script:
	echo "#!/bin/bash" > run_myprogram.sh
	echo "ulimit -s unlimited" >> run_myprogram.sh
	echo "./myprogram" >> run_myprogram.sh
	chmod +x run_myprogram.sh

clean:
	rm -rf $(OBJDIR) myprogram run_myprogram.sh
	$(MAKE) -C $(LIBDIR) clean
