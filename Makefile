
CC = gcc -g -Wall -Werror -pedantic
CC_2 = gcc -g -Wall -pedantic

NCURSES_CFLAGS = `pkg-config --cflags ncursesw`
NCURSES_LIBS =  `pkg-config --libs ncursesw`

LIBS += $(NCURSES_LIBS)
CFLAGS += $(NCURSES_CFLAGS)


SRCS = fm.c
OBJS = $(SRCS: .c = .o)
PROG = fm 

all: $(OJBS) 
		$(CC_2) $(CFLAGS) $(OBJS) -o $(PROG) $(LIBS) 
.c.o:
		$(CC_2) $(CFLAGS) -c $<

clean:
