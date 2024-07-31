CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic -O3
LDLIBS:=
LDFLAGS:= -Iinclude

MODELD:=models/
BIND:=bin/
OBJD:=obj/
SRCD:=src/
SRC:=utils.c obj.c mtl.c color.c illum.c map.c vec.c
SRC:=$(addprefix ${SRCD},${SRC})
OBJ:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRC}))

OUTPUT:=main
OUTPUT:=$(addprefix ${BIND},${OUTPUT})

.PHONY: clean run



main: ${OBJ}
	${LINK.o} ${OBJ} -o ${OUTPUT}
	mkdir ${BIND}/${MODELD}
	cp -a ${MODELD}/. ${BIND}/${MODELD} 

obj/%.o: src/%.c
	${COMPILE.c} ${LDFLAGS} $< -o ${OBJD}/$*.o

clean:
	-rm ${OBJ}
	-rm ${OUTPUT}
	-rm -r ${BIND}/*

run:
	./${OUTPUT}
