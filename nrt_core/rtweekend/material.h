#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

enum MaterialTypes
{
	MATERIAL_LAMBERTIAN,
	MATERIAL_METAL,
	MATERIAL_DIELECTRIC
};

class material
{
public:
	virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const = 0;
	virtual void update(shared_ptr<material> new_material) = 0;

	MaterialTypes type;
};

class lambertian : public material
{
public:
	lambertian(const color &a) : albedo(a) { type = MATERIAL_LAMBERTIAN; }

	virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
	{
		vec3 scatter_direction = rec.normal + random_unit_vector();
		scattered = ray(rec.p, scatter_direction);
		attenuation = albedo;

		return true;
	}

	virtual void update(shared_ptr<material> new_material)
	{
		albedo = std::static_pointer_cast<lambertian>(new_material)->albedo;
	}

public:
	color albedo;
};

class metal : public material
{
public:
	metal(const color &a, double f) : albedo(a), fuzz(f < 1 ? f : 1) { type = MATERIAL_METAL; }

	virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return dot(scattered.direction(), rec.normal) > 0;
	}

	virtual void update(shared_ptr<material> new_material)
	{
		shared_ptr<metal> cast = std::static_pointer_cast<metal>(new_material);
		albedo = cast->albedo;
		fuzz = cast->fuzz;
	}

public:
	color albedo;
	double fuzz;
};

double schlick(double cosine, double ref_idx);

class dielectric : public material
{
public:
	dielectric(double ri) : ref_idx(ri) { type = MATERIAL_DIELECTRIC; }

	virtual bool scatter(const ray &r_in, const hit_record &rec, color &attenuation, ray &scattered) const
	{
		attenuation = color(1.0, 1.0, 1.0);
		double etai_over_etat = rec.front_face ? 1.0 / ref_idx : ref_idx;

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		if (etai_over_etat * sin_theta > 1.0)
		{
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}

		double reflect_prob = schlick(cos_theta, etai_over_etat);
		if (random_double() < reflect_prob)
		{
			vec3 reflected = reflect(unit_direction, rec.normal);
			scattered = ray(rec.p, reflected);
			return true;
		}

		vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
		scattered = ray(rec.p, refracted);
		return true;
	}

	virtual void update(shared_ptr<material> new_material)
	{
		shared_ptr<dielectric> cast = std::static_pointer_cast<dielectric>(new_material);
		ref_idx = cast->ref_idx;
	}

	double ref_idx;
};

#endif