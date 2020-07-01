#include "vec3.h"
#include "rtweekend.h"

inline vec3 vec3::random()
{
	return vec3(random_double(), random_double(), random_double());
}

inline vec3 vec3::random(double min, double max)
{
	return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

vec3 random_in_unit_sphere()
{
	while (true)
	{
		vec3 p = vec3::random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

vec3 random_unit_vector()
{
	double a = random_double(0, 2 * pi);
	double z = random_double(-1, 1);
	double r = sqrt(1 - z * z);
	return vec3(r*cos(a), r*sin(a), z);
}

vec3 random_in_hemisphere(const vec3 &normal)
{
	vec3 in_unit_sphere = random_in_unit_sphere();
	if (dot(in_unit_sphere, normal) > 0.0)
	{
		return in_unit_sphere;
	}
	else
	{
		return -in_unit_sphere;
	}
}

vec3 random_in_unit_disk()
{
	while (true)
	{
		vec3 p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}

vec3 reflect(const vec3 &v, const vec3 &n)
{
	return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat)
{
	double cos_theta = dot(-uv, n);
	vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_squared()) * n;
	return r_out_parallel + r_out_perp;
}