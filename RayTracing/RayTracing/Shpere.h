#pragma once
#include "Object.h"
#include "Ray.h"



bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
	float discr = b * b - 4 * a * c;
	if (discr < 0) return false;
	else if (discr == 0) x0 = x1 = -0.5 * b / a;
	else {
		float q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
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
	bool intersect(const Vector &orig, const Vector &dir, float &tnear, uint32_t &index, Vector2D &uv) const
	{
		// analytic solution
		Vector L = orig - center;
		float a = (dir | dir);
		float b = 2 * (dir | L);
		float c = (L | L) - radius2;
		float t0, t1;
		if (!solveQuadratic(a, b, c, t0, t1)) return false;
		if (t0 < 0) t0 = t1;
		if (t0 < 0) return false;
		tnear = t0;

		return true;
	}

	void getSurfaceProperties(const Vector &P, const Vector &I, const uint32_t &index, const Vector2D &uv, Vector &N, Vector2D &st) const
	{
		N = (P - center).GetSafeNormal();
	}

	Vector center;
	float radius, radius2;
};

