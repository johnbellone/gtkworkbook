include ../Makefile.base

SRCS=$(wildcard *.cc) $(wildcard concurrent/*.cc) $(wildcard network/*.cc) $(wildcard proactor/*.cc) 
OBJS=$(patsubst %.cc,${OBJDIR}/%.o,${SRCS})

LFLAGS=-lgtkworkbook -lgtkextra-x11-2.0 -lgthread-2.0

all: 	gtkworkbook
#	make -C realtime/ all
	make -C largefile/ all

gtkworkbook: ${OBJS}
	${CX} -o ${PROJDIR}/bin/gtkworkbook ${OBJS} ${LIBS} ${LFLAGS}

clean:		
	${MD} ${OBJDIR}	
	${RM} ${OBJS} ${INSBINDIR}/gtkworkbook
#	make -C realtime/ clean
	make -C largefile/ clean

install: all
	${MD} ${INSBINDIR}
	${CP} ${PROJDIR}/bin/gtkworkbook ${INSBINDIR}/gtkworkbook
#	make -C realtime/ install
	make -C largefile/ install

${OBJDIR}/%.o: %.cc
	${CX} ${INCS} -c ${CXFLAGS} $< -o $@

.cc.o:
	${CX} ${INCS} -c ${CXFLAGS} $<
