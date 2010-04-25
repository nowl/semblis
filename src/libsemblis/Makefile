CC = gcc
CCFLAGS = -Wall -g -fpic
#CCFLAGS = -Wall -g -O2 -fpic
INCLUDES = -I../../include
LDFLAGS = -shared
LIBS =

SRCS = classifier.c \
       engine.c \
       hashtable.c \
       output.c \
       utils.c \
       config.c \
       environment.c \
       lib_loader.c \
       pretty_printer.c \
       core_prims.c \
       eval.c \
       logger.c \
       reader.c \
       data.c \
       gc.c \
       search_dir.c

OBJS = $(SRCS:.c=.o)

MAIN = libsemblis.so


.SUFFIXES: .c .o

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: .depend clean

$(MAIN): $(OBJS)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o *~ $(MAIN)

.depend: $(SRCS)
	$(CC) -M $(CCFLAGS) $(INCLUDES) $^ > $@

include .depend