#include "color.h"

inline int_color_t
from_floats(float r, float g, float b, float a) {
	return int_color_t { 
		.hex = (
			  (((uint32_t)a * 255)) 
			| (((uint32_t)b * 255) << 8) 
			| (((uint32_t)g * 255) << 16) 
			| (((uint32_t)r * 255) << 24)
		)
	};
}

inline float_color_t
from_ints(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	return float_color_t { 
		.r = ((float)r / 255.0f), 
		.g = ((float)g / 255.0f),
		.b = ((float)b / 255.0f),
		.a = ((float)a / 255.0f)
	};
}

inline float_color_t
from_int(uint32_t hex) {
	return float_color_t {
		.r = ((float)(hex & 0x000000ff) / 255.0f),
		.g = ((float)(hex & 0x0000ff00) / 255.0f),
		.b = ((float)(hex & 0x00ff0000) / 255.0f),
		.a = ((float)(hex & 0xff000000) / 255.0f),
	};
}

color_t
mul(const color_t* c1, const color_t* c2) {
	color_t res;
	
	return res;
}