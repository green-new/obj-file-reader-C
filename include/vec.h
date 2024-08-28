/**
 * @file vec.h
 * @author green
 * @date 7/30/2024
 * @brief Structures and functions for 3 dimension vectors.
 * Header file containing the definitions of 3 dimensional vectors, as well as
 * the mathematical functions surrounding linear algebra and 3d math.
 */

/**
 * @struct vec_t
 * @brief 3-float vector.
 */
typedef struct {
	float x;
	float y;
	float z;
} vec_t;

/** 
 * @brief Cross multiplication.
 * @param a First operand.
 * @param b Second operand.
 * @return The result of cross multiplying a * b.
 */
vec_t
cross(const vec_t* a, const vec_t* b);

/** 
 * @brief Dot product.
 * @param a First operand.
 * @param b Second operand.
 * @return The result of the dot product a and b.
 */
float
dot(const vec_t* a, const vec_t* b);

/** 
 * @brief Vector addition, component wise.
 * @param a First operand.
 * @param b Second operand.
 * @return The result of adding a + b.
 */
vec_t
add(const vec_t* a, const vec_t* b);

/** 
 * @brief Vector subtraction, component-wise.
 * @param a First operand.
 * @param b Second operand.
 * @return The result of adding a - b.
 */
vec_t
sub(const vec_t* a, const vec_t* b);

/** 
 * @brief Vector magnitude.
 * @param a First operand.
 * @return The magnitude of vector a.
 */
float
magnitude(const vec_t* a);

/** 
 * @brief Vector normalization.
 * @param a First operand.
 * @return The normal vector of a.
 */
vec_t
normalize(const vec_t* a);