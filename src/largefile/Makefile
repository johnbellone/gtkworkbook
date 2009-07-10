include ../../Makefile.base

INCS += -I/home/johnb/include -I${PROJDIR}/shared -I../
SRCS=$(wildcard *.cpp) 
OBJS=$(patsubst %.cpp,${OBJDIR}/largefile/%.o,${SRCS})

_CXFLAGS= -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

all: 	largefile

largefile: ${OBJS}
	${CX} ${_CXFLAGS} -shared -Wl -o ${PROJDIR}/bin/largefile.so ${OBJS} ${LIBS} \
	-lgthread-2.0 -lpthread -lcsv -lgtkworkbookshared -lgtkworkbook

clean:		
	${MD} ${OBJDIR}/largefile
	${RM} ${OBJS} ${PROJDIR}/bin/largefile.so

install: all
	${MD} ${INSEXTDIR}
	${CP} ${PROJDIR}/bin/largefile.so ${INSEXTDIR}/largefile.so

${OBJDIR}/largefile/%.o: %.cpp
	${CX} ${_CXFLAGS} ${INCS} -c -fPIC ${CXFLAGS} $< -o $@

.cpp.o:
	${CX} ${_CXFLAGS} ${INCS} -c -fPIC ${CXFLAGS} $<ls