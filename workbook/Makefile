include ../Makefile.base

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,${OBJDIR}/libworkbook/%.o,${SRCS})

all: 	libworkbook

install:
	${RM} ${INSLIBDIR}/libworkbook.a ${INSLIBDIR}/libworkbook.so.1 \
	${INSLIBDIR}/libworkbook.so.1
	${CP} ${PROJDIR}/lib/libworkbook.so ${INSLIBDIR}/libworkbook.so.1
	${LN} ${INSLIBDIR}/libworkbook.so.1 ${INSLIBDIR}/libworkbook.a
	${LN} ${INSLIBDIR}/libworkbook.a ${INSLIBDIR}/libworkbook.so
#	chcon -t textrel_shlib_t ${INSLIBDIR}/libworkbook.so.1

libworkbook: ${OBJS}
	${RM} ${PROJDIR}/lib/libworkbook.so.1 ${PROJDIR}/lib/libworkbook.so
	${CC} -shared -Wl,-soname,libworkbook.so.1 \
	-o ${PROJDIR}/lib/libworkbook.so ${OBJS} ${LIBS} \
	-lgtkextra-x11-2.0 -ldl -gthread-2.0
	${LN} ${PROJDIR}/lib/libworkbook.so ${PROJDIR}/lib/libworkbook.so.1
clean:		
	${MD} ${OBJDIR}/libworkbook	
	${RM} ${OBJS} ${PROJDIR}/lib/libworkbook.so.1

${OBJDIR}/libworkbook/%.o: %.c
	${CC} ${INCS} -c -fPIC ${CCFLAGS} $< -o $@

.c.o:
	${CC} ${INCS} -c -fPIC ${CCFLAGS} $<
