#pragma once
#include "CoreMinimal.h"
#include <math.h>
#include "../GCMath/MathHelper.h"
#include "Random.h"

class RTMath:oocd::Math
{
public:
	static float clamp(const float& lo, const float& hi, const float& v)
	{
		return max(lo, min(hi, v));
	}
	static float deg2rad(const float& deg)
	{
		return deg * PI / 180;
	}
	static Vector mix(const Vector& a, const Vector& b, const float& mixValue)
	{
		return a * (1 - mixValue) + b * mixValue;
	}
	
	/*Schlick散射函数根据光与相机方向之间的角度不同地反射光。*/
	static float schlick(float cosine, float ref_idx)
	{
		float r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * Math::Pow((1 - cosine), 5);
	}
	static oocd::Vector refract(const Vector& I, const Vector& N, const float& ior)
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
	static bool refract(const Vector& v, const Vector& n, float ni_over_nt, Vector& refracted)
	{
		Vector uv = v.GetSafeNormal();
		float dt = (uv | n);
		float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);
		if (discriminant > 0) {
			refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
			return true;
		}
		else
			return false;
	}
	static oocd::Vector reflect(const Vector& v, const Vector& n)
	{
		return v - 2 * (v | n) * n;
	}
	static void fresnel(const Vector& I, const Vector& N, const float& ior, float& kr)
	{
		float cosi = clamp(-1, 1, (I | N));
		float etai = 1, etat = ior;
		if (cosi > 0) { std::swap(etai, etat); }
		// Compute sini using Snell's law
		float sint = etai / etat * sqrtf(max(0.f, 1 - cosi * cosi));
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
	static oocd::Vector random_in_unit_sphere()
	{
		Vector p;
		do {
			p = 2.0f * Vector((float)drand48(), (float)drand48(), (float)drand48()) - Vector(1, 1, 1);
		} while (p.Size() >= 1.0f);
		return p;
	}



protected:
private:
};