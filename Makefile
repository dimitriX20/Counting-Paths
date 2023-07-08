CC := g++
PYTHON := python3
CFLAGS := -std=c++17 -O3 -pthread -fsanitize=address
INCLUDES := -I./homlib/src -I/home/dimitri/.local/include -I./nauty2_8_6 -I./homsearch
SRCDIR := ./homlib/src
OBJDIR := ./homlib/obj
LIBDIR := ./nauty2_8_6
LIBS := -L./homsearch -lhomsearch_interface $(LIBDIR)/nauty.a

SOURCES := $(wildcard $(SRCDIR)/*.cc)
OBJECTS := $(patsubst $(SRCDIR)/%.cc, $(OBJDIR)/%.o, $(SOURCES)) $(OBJDIR)/main.o

.PHONY: all clean nauty homsearch

all: $(OBJDIR) nauty homsearch $(OBJECTS) myprogram run_script

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR)/main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

nauty:
	$(MAKE) -C $(LIBDIR)

homsearch:
	cd ./homsearch && $(PYTHON) setup.py build_ext --inplace && \
	ln -sf homsearch_interface.cpython-38-x86_64-linux-gnu.so libhomsearch_interface.so

myprogram: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lpython3.8

run_script:
	echo "#!/bin/bash" > run_myprogram.sh
	echo "ulimit -s unlimited" >> run_myprogram.sh
	echo 'export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:'"$(pwd)/homsearch" >> run_myprogram.sh
	echo "./myprogram" >> run_myprogram.sh
	chmod +x run_myprogram.sh

clean:
	rm -rf $(OBJDIR) myprogram run_myprogram.sh
	$(MAKE) -C $(LIBDIR) clean
	$(MAKE) -C ./homsearch clean
