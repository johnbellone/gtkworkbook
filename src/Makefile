include ../Makefile.base

INCS += -I${PROJDIR}/shared
SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp,${OBJDIR}/%.o,${SRCS})

LFLAGS=-lgtkworkbook -lgtkextra-x11-2.0 -lgthread-2.0 -lgtkworkbookshared

all: 	gtkworkbook
#	make -C realtime/ all
	make -C largefile/ all

gtkworkbook: ${OBJS}
	${CX} ${CXFLAGS} -rdynamic -o ${PROJDIR}/bin/gtkworkbook ${OBJS} ${LIBS} ${LFLAGS}

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

${OBJDIR}/%.o: %.cpp
	${CX} -rdynamic ${INCS} -c ${CXFLAGS} $< -o $@

.cpp.o:
	${CX} -rdynamic ${INCS} -c ${CXFLAGS} $<
