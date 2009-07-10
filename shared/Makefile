include ../Makefile.base

SRCS=$(wildcard *.cc) $(wildcard concurrent/*.cc) \
$(wildcard network/*.cc) $(wildcard proactor/*.cc)
OBJS=$(patsubst %.cc,${OBJDIR}/libgtkworkbookshared/%.o,${SRCS})

all: 	gtkworkbookshared

gtkworkbookshared: ${OBJS}
	${CX} -shared -Wl -o ${PROJDIR}/lib/libgtkworkbookshared.so ${OBJS} ${LIBS} \
	-lpthread
	${CP} ${PROJDIR}/lib/libgtkworkbookshared.so ${PROJDIR}/lib/libgtkworkbookshared.so.1

clean:		
	${MD} ${OBJDIR}/libgtkworkbookshared/concurrent ${OBJDIR}/libgtkworkbookshared/network \
	${OBJDIR}/libgtkworkbookshared/proactor
	${RM} ${OBJS} ${PROJDIR}/lib/libgtkworkbookshared.so ${PROJDIR}/lib/libgtkworkbookshared.so.1

install: all
	${MD} ${INSEXTDIR}

${OBJDIR}/libgtkworkbookshared/%.o: %.cc
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $< -o $@

.cc.o:
	${CX} ${INCS} -c -fPIC ${CXFLAGS} $<