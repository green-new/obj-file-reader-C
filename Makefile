CC:=gcc
CFLAGS:=-g -Wall -std=c99 -pedantic -O0 # -fanalyzer
# LDLIBS:=
LDFLAGS:=-Iinclude

MODELD:=models
BIND:=bin
OBJD:=obj
SRCD:=src
SRCS:=$(wildcard ${SRCD}/*.c)
OBJS:=$(subst .c,.o,$(subst ${SRCD},${OBJD},${SRCS}))

OUTPUT:=libcmtlobj.a
OUTPUT:=$(addprefix ${BIND}/,${OUTPUT})

# list of test modules
TEST_MODULES:=main map mtl object perf token
TEST_MODULES_RUN:=$(addprefix run_,${TEST_MODULES})

.PHONY: all clean ${TEST_MODULES}

# clean, compile, link, then compile testing modules
all: ${OUTPUT} ${TEST_MODULES}

# link each object file into the .a library
${OUTPUT}: ${OBJS}
	ar rcs ${OUTPUT} ${OBJS}

run_tests: ${OUTPUT} ${TEST_MODULES} ${TEST_MODULES_RUN}

${TEST_MODULES_RUN}: $(subst run_,,$@)
	-./test/$(subst run_,,$@)/bin/main.exe > ./test/$(subst run_,,$@)/out/output.txt

# compile each src file
obj/%.o: src/%.c
	${COMPILE.c} ${LDFLAGS} $< -o ${OBJD}/$*.o

# makefile target for each module
${TEST_MODULES}: ${OBJS}
	${MAKE} -C ./test/$@

# clean the project of any binaries
clean:
	@$(foreach mod,${TEST_MODULES},\
	$(MAKE) -C ./test/${mod} clean;)
	-rm -rf ${BIND}/*
	-rm -rf ${OBJD}/*