#include "color.h"
#include <math.h>

inline color_t 
color_bitand(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex & c2->hex;
	return res;
}

inline color_t 
color_bitor(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex | c2->hex;
	return res;
}

inline color_t 
color_bitxor(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex ^ c2->hex;
	return res;
}

inline color_t 
color_add(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex + c2->hex;
	return res;
}

inline color_t 
color_sub(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex - c2->hex;
	return res;
}

inline color_t
color_mul(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex * c2->hex;
	return res;
}

inline color_t 
color_div(const color_t* c1, const color_t* c2) {
	color_t res = {0};
	res.hex = c1->hex / c2->hex;
	return res;
}

inline color_t
color_from_floats(const colorf_t* c1) {
	return (color_t) { 
		.rgba.r = (uint32_t)(c1->r * 255),
		.rgba.g = (uint32_t)(c1->b * 255) << 8,
		.rgba.b = (uint32_t)(c1->g * 255) << 16,
		.rgba.a = (uint32_t)(c1->r * 255) << 24
	};
}

inline color_t
color_from_ints(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (color_t) {
		.rgba.r = r,
		.rgba.g = g,
		.rgba.b = b,
		.rgba.a = a
	};
}

inline color_t
color_from_int(uint32_t hex) {
	return (color_t) {
		.rgba.r = (hex & 0x000000ff),
		.rgba.g = (hex & 0x0000ff00) << 8,
		.rgba.b = (hex & 0x00ff0000) << 16,
		.rgba.a = (hex & 0xff000000) << 24
	};
}