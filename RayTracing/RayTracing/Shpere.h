#pragma once
#include "Object.h"
#include "Ray.h"



inline bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5f * b / a;
	else {
		float q = (b > 0) ?
			-0.5f * (b + sqrt(discr)) :
			-0.5f * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}
	if (x0 > x1) std::swap(x0, x1);
	return true;
}

class Sphere : public Object 
{
public:
	Sphere(const Vector &c, const float &r) : center(c), radius(r), radius2(r * r) {}
	bool intersect(const Vector &orig, const Vector &dir, float &tnear, uint32_t &index, Vector2D &uv) const;	

	void getSurfaceProperties(const Vector &P, const Vector &I, const uint32_t &index, const Vector2D &uv, Vector &N, Vector2D &st) const;
public:
	int		getType() { return SPHERE; }
	Vector& getCentre() { return center; }
	float	getRadius() { return radius; }
	float	getSqRadius() { return radius2; }
	Color	getColor(Vector& p_Pos);

	inline Vector getNormal(Vector& p_Pos) { return (p_Pos - center) / radius; }
	inline Vector getNormal(Vector& p_Pos, Vector& p_RayO)
	{
		Vector ret = p_Pos - center;
		return ret;
	}
public:
	Vector center;
	float radius;
	float radius2;
};

