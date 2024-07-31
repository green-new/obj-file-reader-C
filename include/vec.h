/**
 * @file vec.h
 * @author green
 * @date 7/30/2024
 * @brief Structures and functions for 3 dimension vectors.
 * Header file containing the definitions of 3 dimensional vectors, as well as
 * the mathematical functions surrounding linear algebra and 3d math.
 */

typedef struct {
	float x;
	float y;
	float z;
} vec_t;

vec_t
cross(const vec_t* a, const vec_t* b);

float
dot(const vec_t* a, const vec_t* b);

vec_t
add(const vec_t* a, const vec_t* b);

vec_t
sub(const vec_t* a, const vec_t* b);

float
magnitude(const vec_t* a);

vec_t
normalize(const vec_t* a);