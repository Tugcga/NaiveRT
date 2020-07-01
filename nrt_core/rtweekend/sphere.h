#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere: public hittable
{
public:
	sphere() {}
	sphere(point3 &cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};
	void set_center(const point3 &cen);
	void set_radius(double r);


	virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const;

public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
};

#endif