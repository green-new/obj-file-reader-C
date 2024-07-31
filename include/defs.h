/** @file defs.h
 * @author green
 * @date 7/30/2024
 * @brief Definitions for return codes and their related functions.
 * Header file containing the enumerations of return codes from functions within
 * this library.
 */
#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define MAX_LINE_LEN 255

#if CHAR_BIT != 8
#error "Unsupported char size"
#endif

// Defines for R, G, B, A array access
#ifndef R
#define R 0
#endif
#ifndef G
#define G 1
#endif
#ifndef B
#define B 2
#endif
#ifndef A
#define A 3
#endif

// Defines for X, Y, Z, W array access
#ifndef X
#define X 0
#endif
#ifndef Y
#define Y 1
#endif
#ifndef Z
#define Z 2
#endif
#ifndef W
#define W 3
#endif

enum search_codes {
	FOUND = 0,
	NOT_FOUND = 1
}

enum capacity_codes {
	EMPTY = 0,
	NON_EMPTY = 1
}

/** @brief Integer return codes for this library.
 * Return codes for this library as integers. A successful exit is always 0.
 */
enum return_codes {
    SUCCESS = 0,
    MEMORY_REFUSED,
    INVALID_FILE,
    INVALID_DIMS
};

/** Prints a readable description of a given return code.
 * 
 * @param return_code An integer return code from some of this library's 
 * functions.
 * @return A readable string literal.
 */
static inline char* errstr(int return_code) {
    switch (return_code) {
        case SUCCESS:
            return "Successful termination";
        case OUT_OF_MEMORY:
            return "Out of memory";
        case INVALID_FILE:
            return "Could not open or create file";
        case INVALID_DIMS:
            return "Invalid or inconsistent specified number of dimensions";
        default: break;
    }
    return "Undefined";
}

#endif