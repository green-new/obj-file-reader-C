CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic -O3
# LDLIBS:=
# 'include' is 2 dirs back
LDFLAGS:=-I/../../include

BIND:=bin
OBJD:=obj
SRCD:=src
TEST_MAIN_SRC
SRC:=$(addprefix ${SRCD}/,${SRC})
OBJ:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRC}))

OUTPUT:=main
OUTPUT:=$(addprefix ${BIND}/,${OUTPUT})

.PHONY: clean run

test_main:

test_map:

test_mtl:

test_perf:

test_token:

test_clean:

test_run: