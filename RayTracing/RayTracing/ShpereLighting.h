#pragma once
#include "Light.h"
#include "Shpere.h"
class ShpereLighting :public BaseLighting
{
public:
	ShpereLighting();
	ShpereLighting(Vector Position ,float rad,Color SCol);

	virtual bool intersect(Ray o_ray, float& dist);
	//点光源是无法进行交集计算的
	virtual bool intersect(const Vector&, const Vector&, float&, uint32_t&, Vector2D&) const { return false; };
	Vector& getCentre() { return center; }
	virtual int getType() { return Shpere; };
	float	getRadius() { return radius; }
	float	getSqRadius() { return radius2; }
	inline Vector getNormal(Vector& p_Pos) { return (p_Pos - center) / radius; }
	inline Vector getNormal(Vector& p_Pos, Vector& p_RayO)
	{
		Vector ret = p_Pos - center;
		return ret;
	}
	float radius;
	Vector center;

	float radius2;

};