CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic
# LDLIBS:=
# 'include' is 2 dirs back
LDFLAGS:=-I/../../include

# directories found in each test module
BIND:=bin
OBJD:=obj
SRCD:=src
OUTD:=out

# list of modules
MODULES:=main map mtl obj perf token

.PHONY: all run clean ${MODULES}

all: ${MODULES}

# makefile target for each module
${MODULES}:
	${MAKE} -C $@

# compiles and executes every test module and outputs to 'output.txt'
run: all
	@$(foreach mod, ${MODULES}, \
	echo "TEST [${mod}]" \
	./${mod}/${BIND}/main.exe > ${mod}/${OUTD}/output.txt \
	echo "[${mod}] returned $?")

clean:
	@${foreach mod,${MODULES}, \
	$(MAKE) -C ${mod} clean;}