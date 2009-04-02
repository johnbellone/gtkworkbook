include ../Makefile.base

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,${OBJDIR}/%.o,${SRCS})

LFLAGS=-lworkbook -lconfig -lgtkextra-x11-2.0 -lgthread-2.0

all: 	gtkcsv
	make -C realtime/ all
#	make -C basic/ all
#	make -C csv/ all

gtkcsv: ${OBJS}
	${CC} -o ${PROJDIR}/bin/gtkcsv ${OBJS} ${LIBS} ${LFLAGS}

clean:		
	${MD} ${OBJDIR}	
	${RM} ${OBJS} ${INSBINDIR}/gtkcsv
	make -C realtime/ clean
#	make -C basic/ clean
#	make -C csv/ clean

install: all
	${MD} ${INSBINDIR}
	${CP} ${PROJDIR}/bin/gtkcsv ${INSBINDIR}/gtkcsv
	make -C realtime/ install
#	make -C basic/ install
#	make -C csv/ install

${OBJDIR}/%.o: %.c
	${CC} ${INCS} -c ${CCFLAGS} $< -o $@

.c.o:
	${CC} ${INCS} -c ${CCFLAGS} $<
