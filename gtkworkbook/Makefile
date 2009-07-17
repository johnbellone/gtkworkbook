include ../Makefile.base

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,${OBJDIR}/libgtkworkbook/%.o,${SRCS})

all: 	libgtkworkbook

install:
	${RM} ${INSLIBDIR}/libgtkworkbook.a ${INSLIBDIR}/libgtkworkbook.so.1 \
	${INSLIBDIR}/libgtkworkbook.so.1
	${CP} ${PROJDIR}/lib/libgtkworkbook.so ${INSLIBDIR}/libgtkworkbook.so.1
	${LN} ${INSLIBDIR}/libgtkworkbook.so.1 ${INSLIBDIR}/libgtkworkbook.a
	${LN} ${INSLIBDIR}/libgtkworkbook.a ${INSLIBDIR}/libgtkworkbook.so
#	chcon -t textrel_shlib_t ${INSLIBDIR}/libgtkworkbook.so.1

libgtkworkbook: ${OBJS}
	${MD} ${PROJDIR}/lib
	${RM} ${PROJDIR}/lib/libgtkworkbook.so.1 ${PROJDIR}/lib/libgtkworkbook.so
	${CC} -shared -Wl,-soname,libgtkworkbook.so.1 \
	-o ${PROJDIR}/lib/libgtkworkbook.so ${OBJS} ${LIBS} \
	-lgtkextra-x11-2.0 -ldl -gthread-2.0
	${LN} ${PROJDIR}/lib/libgtkworkbook.so ${PROJDIR}/lib/libgtkworkbook.so.1
clean:		
	${MD} ${OBJDIR}/libgtkworkbook	
	${RM} ${OBJS} ${PROJDIR}/lib/libgtkworkbook.so.1

${OBJDIR}/libgtkworkbook/%.o: %.c
	${CC} ${INCS} -c -fPIC ${CCFLAGS} $< -o $@

.c.o:
	${CC} ${INCS} -c -fPIC ${CCFLAGS} $<
