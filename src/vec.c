#include "vec.h"
#include <math.h>

vec_t
cross(const vec_t* a, const vec_t* b) {
	return vec_t {
		.x = (a->y * a->z) - (a->z * b->y),
		.y = (a->z * b->x) - (a->x * b->z), 
		.z = (a->x * b->y) - (a->y * b->x)
	};
}

float
dot(const vec_t* a, const vec_t* b) {
	return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

vec_t
add(const vec_t* a, const vec_t* b) {
	return vec_t {
		.x = a->x + b->x,
		.y = a->y + b->y,
		.z = a->z + b->z
	};
}

vec_t
sub(const vec_t* a, const vec_t* b) {
	return vec_t {
		.x = a->x - b->x,
		.y = a->y - b->y,
		.z = a->z - b->z
	};
}

float
magnitude(const vec_t* a) {
	return sqrtf((a->x * a->x) + (a->y * a->y) + (a->z * a->z));
}

vec_t
normalize(const vec_t* a) {
	vec_t res = *a;
	float mag = magnitude(a);
	if (mag != 0.0f) {
		res.x = a->x / mag;
		res.y = a->y / mag;
		res.z = a->z / mag;
	}
	return res;
}