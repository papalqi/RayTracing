#pragma once
#include "CoreMinimal.h"


struct Ray;
class material;

enum MaterialType { DIFFUSE_AND_GLOSSY, REFLECTION_AND_REFRACTION, REFLECTION };
struct hit_record
{
	float t;
	Vector p;
	Vector normal;
	material *mat_ptr;
};
class Object
{
public:
	virtual ~Object() {}
	Object() :
		materialType(DIFFUSE_AND_GLOSSY),
		ior(1.3), Kd(0.8), Ks(0.2), diffuseColor(0.2), specularExponent(25) {}


	virtual bool intersect(const Vector &, const Vector &, float &, uint32_t &, Vector2D &) const = 0;
	virtual void getSurfaceProperties(const Vector &, const Vector &, const uint32_t &, const Vector2D &, Vector &, Vector2D &) const = 0;
	virtual Vector evalDiffuseColor(const Vector2D &) const { return diffuseColor; }
	MaterialType materialType;
	float ior;
	float Kd, Ks;
	Vector diffuseColor;
	float specularExponent;
};