#include "material.h"

double schlick(double cosine, double ref_idx)
{
	double r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}