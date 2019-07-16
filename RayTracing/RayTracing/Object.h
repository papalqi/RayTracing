#pragma once
#include "CoreMinimal.h"
#include "material.h"
//对象

struct Ray;


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
	enum
	{
		SPHERE = 1,
		PLANE = 2,
		BOX = 3,
		TRIANGLE = 4
	};
	Object() :
		materialType(DIFFUSE_AND_GLOSSY),
		ior(1.3f), Kd(0.8f), Ks(0.2f), diffuseColor(0.2f), specularExponent(25.0f) {}

	virtual bool intersect(Ray o_ray, float &dist) = 0;
	virtual bool intersect(const Vector &, const Vector &, float &, uint32_t &, Vector2D &) const = 0;
	//virtual void getSurfaceProperties(const Vector &, const Vector &, const uint32_t &, const Vector2D &, Vector &, Vector2D &) const;
	virtual Vector evalDiffuseColor(const Vector2D &) const { return diffuseColor; }
public:
	 material* getMaterial() { return &m_Material; }
	 void setMaterial(material& p_Mat) { m_Material = p_Mat; }
	virtual int getType() = 0;
	virtual Vector getNormal(Vector& p_Pos) = 0;
	virtual Vector getNormal(Vector& p_Pos, Vector& p_RayO) { return getNormal(p_Pos); }

	virtual Color getColor(Vector& p_Pos) { return m_Material.getColor(); }

public:
	inline string getName() { return Name; }
	void setName(string other) {Name = other;};
	inline bool IsLight() { return m_Light; }
	virtual void SetLighting(bool p_Light) { m_Light = p_Light; }
public:
	bool m_Light;//是否接受光照
	string Name;
	material m_Material;
	MaterialType materialType;
	float ior;
	float Kd, Ks;
	Vector diffuseColor;
	float specularExponent;
};