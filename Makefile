CC = gcc
CFLAGS = -Wall -Werror -fpic -pedantic 
LIBSDIR = -L. -L/usr/lib
INCLUDEDIR = -I. -I/usr/include

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb -DDEBUG
endif

SOURCEDIR=.
BUILDDIR=build

SOURCES = $(wildcard $(SOURCEDIR)/*.c)
OBJECTS = $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))


EXECUTABLE = main
LIBCORENAME = mappeReducer
LIBTARGET:=lib$(LIBCORENAME:=.so)

all: dir $(OBJECTS) $(LIBTARGET) $(EXECUTABLE)

dir:
	@mkdir -p $(BUILDDIR)

run: all
	./$(EXECUTABLE)

$(EXECUTABLE): $(LIBTARGET)
	$(CC) $(CFLAGS) $(INCLUDEDIR) $(LIBSDIR) -o $(EXECUTABLE) -l$(LIBCORENAME) -lm

$(LIBTARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -shared $(OBJECTS) -o $(LIBTARGET)

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

ci: all
	$(CC) $(CFLAGS) $(INCLUDEDIR) $(LIBSDIR) $(OBJECTS) -o $(EXECUTABLE) -lm

test: ci
	valgrind --track-origins=yes ./$(EXECUTABLE)

binpack: all
	tar -czf binpack-$(LIBCORENAME).tgz $(LIBTARGET) main

clean:
	@rm -rf $(BUILDDIR) *.so main *.tgz
