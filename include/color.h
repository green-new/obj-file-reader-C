/**
 * @file color.h
 * @author green
 * @date 7/30/2024
 * @brief Specifies the color type and related functions.
 * Header file specifying a unionized, integer RGBA color type, and the related 
 * functions converting it to doubles or integers.
 */

#ifndef COLOR_H_INCLUDED
#define COLOR_H_INCLUDED

/** @union color_t
 * @brief A 4-byte union containing a hexadecimal color and its components.
 * A 4-byte color type. The current system must support a 8-bit char type.
 */
typedef union {
	/** Hexadecimal color value. */
	uint32_t hex;
	/** Array of 4 bytes giving the R, G, B, A values in order. R may or may not
	be the 0th index or the 3rd index and is system dependent. */
	uint8_t arr[4];
	/** @struct rgba
	* @brief Contains the R, G, B, A values individually, sharing the same
	* memory as "hex".
	* Unfortunately, depending on the system or byte order, the components may 
	* or may not be representative of their name.
	*/
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

