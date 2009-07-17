include ../../Makefile.base

INCS += -I${PROJDIR}/shared -I../
SRCS = $(wildcard *.cpp) 
OBJS = $(patsubst %.cpp,${OBJDIR}/realtime/%.o,${SRCS}) 

all: 	realtime

realtime: ${OBJS}
	${CX} -shared -Wl -o ${PROJDIR}/bin/realtime.so ${OBJS} ${LIBS} \
	-lgthread-2.0 -lpthread -lcsv

clean:		
	${MD} ${OBJDIR}/realtime
	${RM} ${OBJS} ${PROJDIR}/bin/realtime.so

install: all
	${MD} ${INSEXTDIR}
	${CP} ${PROJDIR}/bin/realtime.so ${INSEXTDIR}/realtime.so

${OBJDIR}/realtime/%.o: %.cpp
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $< -o $@

.cpp.o:
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $<