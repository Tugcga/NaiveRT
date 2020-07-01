#include "converter.h"

vec3 to_vec3(MATH::CVector3 vector)
{
	return vec3(vector.GetX(), vector.GetY(), vector.GetZ());
}