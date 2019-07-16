#include "Shpere.h"



bool Sphere::intersect(const Vector& orig, const Vector& dir, float& tnear, uint32_t& index, Vector2D& uv) const
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

void Sphere::getSurfaceProperties(const Vector& P, const Vector& I, const uint32_t& index, const Vector2D& uv, Vector& N, Vector2D& st) const
{
	N = (P - center).GetSafeNormal();
}

Color Sphere::getColor(Vector& p_Pos)
{
	if (m_Material.getTexure() == NULL)
	{
		return m_Material.getColor();
	}
	Vector r_Pos = p_Pos - center;
	double s = acos(r_Pos.Z / radius) / PI;
	double t = acos(r_Pos.X / (radius * sin(PI * s))) / PI;
	return m_Material.getTexure()->getUVColor(s, t);
}
