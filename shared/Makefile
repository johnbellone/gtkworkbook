include ../Makefile.base

SRCS=$(wildcard *.cpp) $(wildcard concurrent/*.cpp) \
$(wildcard network/*.cpp) $(wildcard proactor/*.cpp)
OBJS=$(patsubst %.cpp,${OBJDIR}/libshared/%.o,${SRCS})

all: 	shared

shared: ${OBJS}
	${CX} -shared -Wl -o ${PROJDIR}/lib/libshared.so ${OBJS} ${LIBS} \
	-lpthread
	${CP} ${PROJDIR}/lib/libshared.so ${PROJDIR}/lib/libshared.so.1

clean:		
	${MD} ${OBJDIR}/libshared/concurrent ${OBJDIR}/libshared/network \
	${OBJDIR}/libshared/proactor
	${RM} ${OBJS} ${PROJDIR}/lib/libshared.so ${PROJDIR}/lib/libshared.so.1

install: all
	${MD} ${INSEXTDIR}

${OBJDIR}/libshared/%.o: %.cpp
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $< -o $@

.cpp.o:
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $<