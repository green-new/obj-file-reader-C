CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic -O3
# LDLIBS:=
LDFLAGS:=-Iinclude

MODELD:=models
BIND:=bin
OBJD:=obj
SRCD:=src
SRC:=utils.c obj.c mtl.c color.c illum.c material_map.c vec.c light.c
SRC:=$(addprefix ${SRCD}/,${SRC})
OBJ:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRC}))

OUTPUT:=main
OUTPUT:=$(addprefix ${BIND}/,${OUTPUT})

.PHONY: clean run


main: ${OBJ}
	${LINK.o} ${OBJ} -o ${OUTPUT}
	-cp -a ${MODELD}/. ${BIND}/${MODELD} 

mkdir:
	-mkdir ${BIND}
	-mkdir ${OBJD}

obj/%.o: src/%.c mkdir
	${COMPILE.c} ${LDFLAGS} $< -o ${OBJD}/$*.o

clean:
	-rm -f ${OBJ}
	-rm -f ${OUTPUT}
	-rm -rf ${BIND}/*
	-rmdir --ignore-fail-on-non-empty ${BIND}/
	-rm -rf ${OBJD}/*
	-rmdir --ignore-fail-on-non-empty ${OBJD}/

run:
	./${OUTPUT}
