include ../../Makefile.base

SRCS=$(wildcard *.cpp) 
OBJS=$(patsubst %.cpp,${OBJDIR}/largefile/%.o,${SRCS})

all: 	largefile

largefile: ${OBJS}
	${CX} -shared -Wl -o ${PROJDIR}/bin/largefile.so ${OBJS} ${LIBS} \
	-lgthread-2.0 -lpthread -lcsv

clean:		
	${MD} ${OBJDIR}/largefile
	${RM} ${OBJS} ${PROJDIR}/bin/largefile.so

install: all
	${MD} ${INSEXTDIR}
	${CP} ${PROJDIR}/bin/largefile.so ${INSEXTDIR}/largefile.so

${OBJDIR}/largefile/%.o: %.cpp
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $< -o $@

.cpp.o:
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $<ls