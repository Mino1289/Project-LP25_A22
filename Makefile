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


EXESOURCE = main
EXECUTABLE = $(EXESOURCE:=.exe)
LIBCORENAME = mappeReducer
LIBTARGET:=lib$(LIBCORENAME:=.so)

all: dir $(OBJECTS) $(EXECUTABLE) $(LIBTARGET)

dir:
	@mkdir -p $(BUILDDIR)

run: all
	@export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
	./$(EXECUTABLE)

$(EXECUTABLE): $(LIBTARGET)
	$(CC) $(CFLAGS) $(INCLUDEDIR) $(LIBSDIR) -o $(EXECUTABLE) -l$(LIBCORENAME) -lm

$(LIBTARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -shared $(OBJECTS) -o $(LIBTARGET)

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BUILDDIR) *.exe *.so
