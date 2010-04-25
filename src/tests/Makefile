CC = gcc
CCFLAGS = -Wall -g
#CCFLAGS = -Wall -g -O2
INCLUDES = -I../../include
LDFLAGS = -L. -Wl,-rpath,'$$ORIGIN'
LIBS = -lsemblis -ldl

SRCS = main.c

OBJS = $(SRCS:.c=.o)

MAIN = semblis


.SUFFIXES: .c .o

.c.o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: .depend clean

$(MAIN): $(OBJS)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS) $(LDFLAGS)

clean:
	rm -f *.o *~ $(MAIN) libsemblis.so

.depend: $(SRCS)
	$(CC) -M $(CCFLAGS) $(INCLUDES) $^ > $@

include .depend