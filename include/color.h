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

/**
 * @brief A union color type.
 * A 4-byte integer color type. The current system must support a 8-bit char 
 * type.
 */
typedef union {
	/** 
	 * Hexadecimal color value. 
	 */
	uint32_t hex;
	/**
	 * @brief Struct containing individual RGBA values.
	 */
	struct {
		/** 
		 * Red value. 
		 */
		uint8_t r;
		/** 
		 * Green value.
		 */
		uint8_t g;
		/** 
		 * Blue value.
		 */
		uint8_t b;
		/** 
		 * Alpha/unused value.
		 */
		uint8_t a;
	} rgba;
} colori_t;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} colorf_t;

color_t
mul(const color_t* c1, const color_t* c2);

int_color_t
from_floats(float r, float g, float b, float a);

float_color_t
from_ints(uint8_t r, uint8_t g, uint8_t b, int8_t a);

float_color_t
from_int(uint32_t hex);

#endif

