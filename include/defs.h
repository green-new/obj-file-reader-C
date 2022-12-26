#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#define MAX_LINE_LEN 255

/**
 * @brief An enumeration of integer return codes for this library. SUCCESS is always 0.
 * 
 */
enum return_codes {
    SUCCESS = 0,
    OUT_OF_MEMORY,
    INVALID_FILE,
    INVALID_DIMS
};

/**
 * @brief Prints a readable description of a given return code.
 * 
 * @param return_code An integer return code from some of this library's functions.
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