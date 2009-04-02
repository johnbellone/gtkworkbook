include ../../Makefile.base

SRCS=$(wildcard *.cpp) $(wildcard concurrent/*.cpp) $(wildcard network/*.cpp) \
	$(wildcard proactor/*.cpp)
OBJS=$(patsubst %.cpp,${OBJDIR}/realtime/%.o,${SRCS})

all: 	realtime

realtime: ${OBJS}
	${CX} -shared -Wl -o ${PROJDIR}/bin/realtime.so ${OBJS} ${LIBS} \
	-lgthread-2.0 -lpthread

clean:		
	${MD} ${OBJDIR}/realtime/concurrent ${OBJDIR}/realtime/network \
	${OBJDIR}/realtime/proactor
	${RM} ${OBJS} ${PROJDIR}/bin/realtime.so

install: all
	${MD} ${INSEXTDIR}
	${CP} ${PROJDIR}/bin/realtime.so ${INSEXTDIR}/realtime.so

${OBJDIR}/realtime/%.o: %.cpp
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $< -o $@

.cpp.o:
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $<ls