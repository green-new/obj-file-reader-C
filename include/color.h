/**
 * @file color.h
 * @author green
 * @date 8/25/2024
 * @brief Specifies the color type and related functions.
 * Header file specifying a unionized, integer RGBA color type, and the related 
 * functions converting it to doubles or integers.
 */

#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

#include <stdint.h>

/** 
 * @brief A union color type.
 * A 4-byte integer color type. The current system must support a 8-bit char 
 * type.
 */
typedef union {
	/** Hexadecimal color value. */
	uint32_t hex;
	/** @brief Struct containing individual RGBA values. */
	struct {
		/** Red value. */
		uint8_t r;
		/** Green value. */
		uint8_t g;
		/** Blue value. */
		uint8_t b;
		/** Alpha/unused value. */
		uint8_t a;
	} rgba;
} color_t;

/** 
 * @brief A color type using normalized float values.
 */
typedef struct {
	/** Red value. */
	float r;
	/** Green value. */
	float g;
	/** Blue value. */
	float b;
	/** Alpha/unused value. */
	float a;
} colorf_t;

/**
 * @brief Get the bitwise AND color between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t 
color_bitand(const color_t* c1, const color_t* c2);

/**
 * @brief Get the bitwise OR color between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t 
color_bitor(const color_t* c1, const color_t* c2);

/**
 * @brief Get the bitwise XOR color between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t 
color_bitxor(const color_t* c1, const color_t* c2);

/**
 * @brief Get the component-wise addition between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t 
color_add(const color_t* c1, const color_t* c2);

/**
 * @brief Get the component-wise subtraction between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t 
color_sub(const color_t* c1, const color_t* c2);

/**
 * @brief Get the component-wise multiplication between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t
color_mul(const color_t* c1, const color_t* c2);

/**
 * @brief Get the component-wise division between two colors.
 * @param c1 The first operand.
 * @param c2 The second operand.
 * @return The color.
 */
color_t 
color_div(const color_t* c1, const color_t* c2);

/**
 * @brief Get the color from the normalized r, g, b, a values.
 * @param c1 The first operand.
 * @return The color.
 */
color_t
color_from_floats(const colorf_t* c1);

/**
 * @brief Get the color from the integer 0-255 rgba values.
 * @param c1 The first operand.
 * @return The color.
 */
color_t
color_from_ints(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

/**
 * @brief Get the color from the integer hex RGBA value.
 * @param c1 The first operand.
 * @return The color.
 */
color_t 
color_from_int(uint32_t hex);

#endif

