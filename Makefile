CXX = gcc
CFLAGS = -Wall -Werror -fpic -pedantic 
LIBSDIR = -L.
INCLUDEDIR = -I.

LIBCORENAME = projet

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb -DDEBUG
endif

ifeq ($(OS), Windows_NT)
	EXPORT = export.bat
	LIBTARGET :=$(LIBCORENAME:=.dll)
	CLEANCMD = @del /q *.o *.dll *.exe *.so main.txt
else
	EXPORT = sh export.sh
	LIBTARGET :=lib$(LIBCORENAME:=.so)
	LIBSDIR += -L/usr/lib
	INCLUDEDIR += -I/usr/include
	CLEANCMD = rm -rf *.o *.so *.exe *.dll 
endif

LIBSOURCE = analysis configuration direct_fork fifo_processes mq_processes reducers utility
LIBSOURCECFILE = $(LIBSOURCE:=.c)
LIBSOURCEOFILE = $(LIBSOURCE:=.o)

EXESOURCE = main
TARGET = $(EXESOURCE:=.exe)
EXESOURCECFILE = $(EXESOURCE:=.c)
EXESOURCEOFILE = $(EXESOURCE:=.o)

all: $(TARGET)

run: $(TARGET)
	$(EXPORT) $(TARGET)

$(TARGET): $(EXESOURCEOFILE) $(LIBTARGET) 
	$(CXX) $(EXESOURCEOFILE) -l$(LIBCORENAME) $(LIBSDIR) -o $(TARGET) -lm

$(LIBTARGET): $(LIBSOURCEOFILE) 
	$(CXX) $(CFLAGS) -shared $(LIBSOURCEOFILE) -o $(LIBTARGET)

.c.o:
	$(CXX) $(CFLAGS) $(INCLUDEDIR) -c -o $@ $<

clean: 
	$(CLEANCMD)
	@echo CLEAN