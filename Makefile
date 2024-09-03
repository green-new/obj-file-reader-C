CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic -O3 # -fanalyzer
# LDLIBS:=
LDFLAGS:=-Iinclude

MODELD:=models
BIND:=bin
OBJD:=obj
SRCD:=src
SRCS:=$(wildcard ${SRCD}/*.c)
OBJS:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRCS}))

OUTPUT:=libmtlobj.a
OUTPUT:=$(addprefix ${BIND}/,${OUTPUT})

.PHONY: clean mkdir

main: clean ${OBJS}
	ar rcs ${OUTPUT} ${OBJS}

mkdir:
	-mkdir ${BIND}
	-mkdir ${OBJD}

obj/%.o: src/%.c mkdir
	${COMPILE.c} ${LDFLAGS} $< -o ${OBJD}/$*.o

clean:
	-rm -rf ${BIND}/*
	-rm -rf ${OBJD}/*