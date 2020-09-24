#include "utils.h"
#include <assert.h>

/**
 * \brief Clamps given integer to given min/max.
 * 
 * \param val Integer to be clamped
 * \param min Minimum value
 * \param max Maximum value
 */
void clamp(int* val, const int min, const int max)
{
	assert(min < max);
	if (*val < min)
		*val = min;
	else if (*val > max)
		*val = max;
}

/**
 * \brief Clamps given unsigned integer to given min/max.
 *
 * \param val Unsigned integer to be clamped
 * \param min Minimum value
 * \param max Maximum value
 */
void clampu(uint* val, const uint min, const uint max)
{
	assert(min < max);
	if (*val < min)
		* val = min;
	else if (*val > max)
		* val = max;
}

/**
 * \brief Clamps given float to given min/max.
 *
 * \param val Float to be clamped
 * \param min Minimum value
 * \param max Maximum value
 */
void clampf(float* val, const float min, const float max)
{
	assert(min < max);
	if (*val < min)
		* val = min;
	else if (*val > max)
		* val = max;
}

/**
 * \brief Clamps given double to given min/max.
 *
 * \param val Double to be clamped
 * \param min Minimum value
 * \param max Maximum value
 */
void clampd(double* val, const double min, const double max)
{
	assert(min < max);
	if (*val < min)
		* val = min;
	else if (*val > max)
		* val = max;
}

/**
 * \brief Normalises given float to a value between given min/max.
 * 
 * \param val Float to be normalised
 * \param min Minimum, default 0.0f
 * \param max Maximum, default 1.0f
 * \return Normalised float value
 */
const float normalise
(const float val, const float min = 0.0f, const float max = 1.0f)
{
	return (val - min) / (max - min);
}

/**
 * \brief Converts red, green and blue RGB channels to hexidecimal 
 * representation.
 *
 * \param r 8 bit unsigned char representing 0-255 of red channel
 * \param g 8 bit unsigned char representing 0-255 of green channel
 * \param b 8 bit unsigned char representing 0-255 of blue channel
 * \return Hexidecimal result stored in an unsigned int 
 * (first 24bits represent hexidecimal)
 */
uint rgbToHex(const uint8 r, const uint8 g, const uint8 b)
{
	uint h = 0x0;
	h += r << 16;
	h += g << 8;
	h += b << 0;
	return h;
}