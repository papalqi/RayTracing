#include "Plane.h"



PlaneObj::PlaneObj(Vector p_Normal, double p_D) : m_Plane(p_Normal, p_D )
{
	//x_Dir = vector3( m_Plane.N.y, m_Plane.N.z, -m_Plane.N.x );
	x_Dir = m_Plane.N ^ (Vector(0, 1, 0));
	y_Dir = x_Dir ^ (m_Plane.N);
	x_Dir.Normalize();
	y_Dir.Normalize();
	O_H = -m_Plane.N | m_Plane.D;
}

Color PlaneObj::getColor(Vector& p_Pos)
{
	if (m_Material.getTexure() == NULL)
	{
		return m_Material.getColor();
	}
	float u = (p_Pos|x_Dir) * m_Material.getTexRatioDao();
	float v = (p_Pos|y_Dir) * m_Material.getTexRatioDao();
	return m_Material.getTexure()->getUVColor(u, v);
}

bool PlaneObj::intersect(const Vector& orig, const Vector& dir, float& tnear, uint32_t& index, Vector2D& uv) const
{
	return false;
}

bool PlaneObj::intersect(Ray o_ray, float& dist)
{
	double d = (m_Plane.N|o_ray.direction());
	if (d != 0)
	{
		double Pdist = -((m_Plane.N|o_ray.origin()) + m_Plane.D) / d;
		if (Pdist > 0)
		{
			if (Pdist < dist)
			{
				dist = Pdist;
				return true;
			}
		}
	}
	return false;
}
