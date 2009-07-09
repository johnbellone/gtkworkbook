include ../Makefile.base

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,${OBJDIR}/%.o,${SRCS})

LFLAGS=-lgtkworkbook -lgtkextra-x11-2.0 -lgthread-2.0

all: 	gtkworkbook
	make -C realtime/ all
	make -C largefile/ all

gtkworkbook: ${OBJS}
	${CC} -o ${PROJDIR}/bin/gtkworkbook ${OBJS} ${LIBS} ${LFLAGS}

clean:		
	${MD} ${OBJDIR}	
	${RM} ${OBJS} ${INSBINDIR}/gtkworkbook
	make -C realtime/ clean
	make -C largefile/ clean

install: all
	${MD} ${INSBINDIR}
	${CP} ${PROJDIR}/bin/gtkworkbook ${INSBINDIR}/gtkworkbook
	make -C realtime/ install
	make -C largefile/ install

${OBJDIR}/%.o: %.c
	${CC} ${INCS} -c ${CCFLAGS} $< -o $@

.c.o:
	${CC} ${INCS} -c ${CCFLAGS} $<
