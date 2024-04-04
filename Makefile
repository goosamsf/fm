
CC = gcc -g -Wall -Werror -pedantic


NCURSES_CFLAGS = `pkg-config --cflags ncursesw`
NCURSES_LIBS =  `pkg-config --libs ncursesw`

LIBS += $(NCURSES_LIBS)
CFLAGS += $(NCURSES_CFLAGS)


SRCS = fm.c
OBJS = $(SRCS: .c = .o)
PROG = fm 

all: $(OJBS) 
		$(CC) $(CFLAGS) $(OBJS) -o $(PROG) $(LIBS) 
.c.o:
		$(CC) $(CFLAGS) -c $<

clean:
