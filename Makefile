CC := g++
PYTHON := python3
CFLAGS := -std=c++17 -O3 -pthread -fsanitize=address
INCLUDES := -I./homlib/src -I/home/dimitri/.local/include -I./nauty2_8_6 -I./homsearch
SRCDIR := ./homlib/src
OBJDIR := ./homlib/obj
LIBDIR := ./nauty2_8_6
LIBS := -L./homsearch -lhomsearch_interface -lpython3.8 $(LIBDIR)/nauty.a


SOURCES := $(wildcard $(SRCDIR)/*.cc)
OBJECTS := $(patsubst $(SRCDIR)/%.cc, $(OBJDIR)/%.o, $(SOURCES))

.PHONY: all clean nauty homsearch

all: nauty homsearch myprogram run_script

$(OBJDIR)/%.o: $(SRCDIR)/%.cc | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

nauty:
	$(MAKE) -C $(LIBDIR)

homsearch:
	cd ./homsearch && $(PYTHON) setup.py build_ext --inplace && \
	ln -sf homsearch_interface.cpython-38-x86_64-linux-gnu.so libhomsearch_interface.so

myprogram: $(OBJECTS) $(OBJDIR)/main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/main.o: main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run_script:
	echo "#!/bin/bash" > run_myprogram.sh
	echo "ulimit -s unlimited" >> run_myprogram.sh
	echo 'export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:'"$(shell pwd)/homsearch" >> run_myprogram.sh
	echo "./myprogram" >> run_myprogram.sh
	chmod +x run_myprogram.sh


clean:
	rm -rf $(OBJDIR) myprogram run_myprogram.sh
	$(MAKE) -C $(LIBDIR) clean
	$(MAKE) -C ./homsearch clean
