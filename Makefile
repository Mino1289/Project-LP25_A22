CC = gcc
CFLAGS = -Wall -Werror -fpic -pedantic
LIBSDIR = -L. -L/usr/lib
INCLUDEDIR = -I. -I/usr/include

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb -DDEBUG
endif

MQ ?= 0
ifeq ($(MQ), 1)
	CFLAGS += -DMQ
endif

DIRECT ?= 0
ifeq ($(DIRECT), 1)
	CFLAGS += -DDIRECT
endif

FIFO ?= 0
ifeq ($(FIFO), 1)
	CFLAGS += -DFIFO
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
	$(CC) $(CFLAGS) $(INCLUDEDIR) $(LIBSDIR) $(BUILDDIR)/$(EXECUTABLE:=.o) -o $(EXECUTABLE) -l$(LIBCORENAME) -lm

$(LIBTARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -shared $(filter-out $(BUILDDIR)/main.o,$(OBJECTS)) -o $(LIBTARGET)

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

ci: all
	$(CC) $(CFLAGS) $(INCLUDEDIR) $(LIBSDIR) $(OBJECTS) -o $(EXECUTABLE:=.exe) -lm

test: ci
	valgrind --track-origins=yes ./$(EXECUTABLE:=.exe)

clean:
	@rm -rf $(BUILDDIR) *.so main *.tgz *.exe
