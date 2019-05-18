#pragma once
#include "CoreMinimal.h"
#include "Random.h"
#include "Ray.h"
#include "Object.h"
//
inline float clamp(const float &lo, const float &hi, const float &v)
{
	return std::max(lo, std::min(hi, v));
}
inline float deg2rad(const float &deg)
{
	return deg * PI / 180;
}
inline Vector mix(const Vector &a, const Vector& b, const float &mixValue)
{
	return  (1 - mixValue)*a  +  mixValue*b ;
}

float schlick(float cosine, float ref_idx);
//散射
Vector refract(const Vector &I, const Vector &N, const float &ior);

bool refract(const Vector& v, const Vector& n, float ni_over_nt, Vector& refracted);

//反射
Vector reflect(const Vector& v, const Vector& n);


void fresnel(const Vector &I, const Vector &N, const float &ior, float &kr);
//在球体内进行采样
Vector random_in_unit_sphere();



class material {
public:
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const = 0;
};

class lambertian : public material 
{
public:
	lambertian(const Vector& a) : albedo(a) {}
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const;

	Vector albedo;
};

class metal : public material 
{
public:
	metal(const Vector& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const;
	Vector albedo;
	float fuzz;
};

class dielectric : public material 
{
public:
	dielectric(float ri) : ref_idx(ri) {}
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const;

	float ref_idx;
};