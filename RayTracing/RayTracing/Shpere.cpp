#include "Shpere.h"

//求解方程
inline bool solveQuadratic(const float& a, const float& b, const float& c, 
	float& x0, float& x1)
{
	//b平方-4ac
	float discr = b * b - 4 * a * c;
	//如果是负数说明无解，
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


bool Sphere::intersect(const Vector& orig, const Vector& dir, float& tnear, uint32_t& index, Vector2D& uv) const
{
	// analytic solution
	Vector L = orig - center;
	float a = (dir | dir);
	float b = 2 * (dir | L);
	float c = (L | L) - radius2;
	float t0, t1;
	//求解方程，如果是无解直接返回
	if (!solveQuadratic(a, b, c, t0, t1)) return false;
	//如果t0是负数说明在光线的后面，就不顶
	if (t0 < 0) t0 = t1;
	if (t0 < 0) return false;
	tnear = t0;

	return true;
}

bool Sphere::intersect(Ray o_ray, float& dist)
{
	Vector v = o_ray.origin() - center;
	double b = -(v| o_ray.direction());
	double det = (b * b) - (v|v) + radius2;
	bool retval = false;
	if (det > THRESH_QUAT_NORMALIZED)
	{
		det = sqrt(det);
		double i1 = b - det;
		double i2 = b + det;
		if (i2 > THRESH_QUAT_NORMALIZED)
		{
			if (i1 < THRESH_QUAT_NORMALIZED)
			{
				if (i2 < dist)
				{
					dist = i2;
					retval = false;
				}
			}
			else
			{
				if (i1 < dist)
				{
					dist = i1;
					retval = true;
				}
			}
		}
	}
	return retval;
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
