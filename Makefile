CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic
LDLIBS:=
LDFLAGS:= -Iinclude

OBJD:=obj/
SRCD:=src/
SRC:=main.c utils.c obj.c mtl.c
SRC:=$(addprefix ${SRCD},${SRC})
OBJ:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRC}))

OUTPUT:=main

.PHONY: clean run



main: ${OBJ}
	${LINK.o} ${OBJ} -o ${OUTPUT}

obj/%.o: src/%.c
	${COMPILE.c} ${LDFLAGS} $< -o ${OBJD}/$*.o

clean:
	-rm ${OBJ}
	-rm ${OUTPUT}

run:
	./${OUTPUT}
