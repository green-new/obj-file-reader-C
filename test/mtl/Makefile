CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic -O0
LDIR=-L./../../bin
LDLIBS:=-lmtlobj
# 'include' is 2 dirs back
LDFLAGS:=-I./../../include

# directories found in the module
BIND:=bin
OBJD:=obj
SRCD:=src
OUTD:=out

# list of source files
# cannot be recursive, although its possible
SRCS:=$(wildcard ${SRCD}/*.c)

# list of object files
OBJS:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRCS}))

# the output is an executable called 'main'
OUTPUT:=main
OUTPUT:=$(addprefix ${BIND}/,${OUTPUT})

.PHONY: clean run

main: ${OBJS}
	${LINK.o} ${OBJS} ${LDIR} ${LDLIBS} -o ${OUTPUT}

# cleans binaries except src files
clean:
	-rm -rf ${BIND}/*
	-rm -rf ${OBJD}/*
	-rm -rf ${OUTD}/*

# compiles and executes and outputs to 'output.txt'
run:
	./${OUTPUT} > ${OUTD}/output.txt

obj/%.o: src/%.c
	${COMPILE.c} ${LDFLAGS} $< -o ${OBJD}/$*.o