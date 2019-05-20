#pragma once
#include "material.h"

/*Schlick散射函数根据光与相机方向之间的角度不同地反射光。*/
float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*Math::Pow((1 - cosine), 5);
}


oocd::Vector refract(const Vector &I, const Vector &N, const float &ior)
{
	float cosi = clamp(-1, 1, (I | N));
	float etai = 1, etat = ior;
	Vector n = N;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = -N; }
	float eta = etai / etat;
	float k = 1 - eta * eta * (1 - cosi * cosi);
	return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}
bool refract(const Vector& v, const Vector& n, float ni_over_nt, Vector& refracted)
{
	Vector uv = v.GetSafeNormal();
	float dt = (uv | n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
	if (discriminant > 0) {
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
		return false;
}


oocd::Vector reflect(const Vector& v, const Vector& n)
{
	return v - 2 * (v | n)*n;
}

void fresnel(const Vector &I, const Vector &N, const float &ior, float &kr)
{
	float cosi = clamp(-1, 1, (I | N));
	float etai = 1, etat = ior;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1) {
		kr = 1;
	}
	else {
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
}

oocd::Vector random_in_unit_sphere()
{	
	Vector p;
	do {
		p = 2.0*Vector(drand48(), drand48(), drand48()) - Vector(1, 1, 1);
	} while (p.Size() >= 1.0);
	return p;
}

bool lambertian::scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const
{
	Vector target = rec.p + rec.normal + random_in_unit_sphere();
	scattered = Ray(rec.p, target - rec.p);
	attenuation = albedo;
	return true;
}

bool metal::scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const
{
	Vector reflected = reflect(r_in.direction().GetSafeNormal(), rec.normal);
	scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
	attenuation = albedo;
	return (scattered.m_direction | rec.normal > 0);
}

bool dielectric::scatter(const Ray& r_in, const hit_record& rec, Vector& attenuation, Ray& scattered) const
{
	Vector outward_normal;
	Vector reflected = reflect(r_in.direction(), rec.normal);
	float ni_over_nt;
	attenuation = Vector(1.0, 1.0, 1.0);
	Vector refracted;
	float reflect_prob;
	float cosine;
	if ((r_in.direction() | rec.normal) > 0) {
		outward_normal = -rec.normal;
		ni_over_nt = ref_idx;
		//         cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
		cosine = (r_in.direction() | rec.normal) / r_in.direction().Size();
		cosine = sqrt(1 - ref_idx * ref_idx*(1 - cosine * cosine));
	}
	else {
		outward_normal = rec.normal;
		ni_over_nt = 1.0 / ref_idx;
		cosine = -(r_in.direction() | rec.normal) / r_in.direction().Size();
	}
	if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
		reflect_prob = schlick(cosine, ref_idx);
	else
		reflect_prob = 1.0;
	if (drand48() < reflect_prob)
		scattered = Ray(rec.p, reflected);
	else
		scattered = Ray(rec.p, refracted);
	return true;
}
