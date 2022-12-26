@echo off
gcc main.c ./src/obj.c ./src/mtl.c ./src/utils.c -o main -Iinclude -g -Wall -std=c99 -pedantic