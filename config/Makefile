include ../Makefile.base

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,${OBJDIR}/libconfig/%.o,${SRCS})

LFLAGS = -lgobject-2.0 -ldl

all: 	libconfig

install:
	${RM} ${INSLIBDIR}/libconfig.a ${INSLIBDIR}/libconfig.so \
	${INSLIBDIR}/libconfig.so.1
	${CP} ${PROJDIR}/lib/libconfig.so ${INSLIBDIR}/libconfig.so.1
	${LN} ${INSLIBDIR}/libconfig.so.1 ${INSLIBDIR}/libconfig.a
	${LN} ${INSLIBDIR}/libconfig.a ${INSLIBDIR}/libconfig.so
#	chcon -t textrel_shlib_t ${INSLIBDIR}/libconfig.so.1	

libconfig: ${OBJS}
	${RM} ${PROJDIR}/lib/libconfig.so.1
	${CC} -shared -Wl,-soname,libconfig.so.1 \
	-o ${PROJDIR}/lib/libconfig.so ${OBJS} ${LIBS} ${LFLAGS}
	${LN} ${PROJDIR}/lib/libconfig.so ${PROJDIR}/lib/libconfig.so.1

clean:		
	${MD} ${OBJDIR}/libconfig	
	${RM} ${OBJS} ${PROJDIR}/lib/libconfig.so.1

${OBJDIR}/libconfig/%.o: %.c
	${CC} ${INCS} -c -fPIC ${CCFLAGS} $< -o $@

.c.o:
	${CC} ${INCS} -c -fPIC ${CCFLAGS} $<
