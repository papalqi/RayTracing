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
	material* mat_ptr;
};

class Object
{
public:
	enum
	{
		SPHERE = 1,
		PLANE = 2,
		BOX = 3,
		TRIANGLE = 4,
		POINT = 5
	};
	Object() {}

	virtual bool intersect(Ray o_ray, float& dist) = 0;

	//virtual void getSurfaceProperties(const Vector &, const Vector &, const uint32_t &, const Vector2D &, Vector &, Vector2D &) const;
	virtual Vector evalDiffuseColor(const Vector2D&) const { return diffuseColor; }
public:
	material* getMaterial() { return &m_Material; }
	void setMaterial(material& p_Mat) { m_Material = p_Mat; }
	virtual int getType() = 0;
	virtual Vector getNormal(Vector& p_Pos) = 0;
	virtual Vector getNormal(Vector& p_Pos, Vector& p_RayO) { return getNormal(p_Pos); }

	virtual Color getColor(Vector& p_Pos) { return m_Material.getColor(); }
	virtual bool IsLight() { return bLight; };
public:
	inline string getName() { return Name; }
	void setName(string other) { Name = other; };
	virtual bool IsLighting() { return m_Light; }
	virtual void SetLighting(bool p_Light) { m_Light = p_Light; }

	int IsAbsorb()
	{
		return m_Material.GetAttenuation();
	}
	int HasPerfectReflection()
	{
		return m_Material.getDiffRefl() ;

	}
	int HasDiffuseReflection()
	{
		return m_Material.getDiffRefl() ;
	}
	int HasRefraction() 
	{
		return m_Material.getRefraction() ;
	};
public:
	bool m_Light;//是否接受光照
	string Name;
	material m_Material;
	MaterialType materialType;
	Vector position;
	bool bLight;//是否是光源
	Vector diffuseColor;
	float specularExponent;
};