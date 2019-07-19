#include "Plane.h"



PlaneObj::PlaneObj(Vector position, Vector  normal,Vector dx,Vector dy) :position(position), dx(dx), dy(dy),normal(normal)
{
	
	
}

Color PlaneObj::getColor(Vector& p_Pos)
{
	if (m_Material.getTexure() == NULL)
	{
		return m_Material.getColor();
	}
	float u = (p_Pos|dx) * m_Material.getTexRatioDao();
	float v = (p_Pos|dy) * m_Material.getTexRatioDao();
	return m_Material.getTexure()->getUVColor(u, v);
}



bool PlaneObj::intersect(Ray ray, float& dist)
{

	float tmp = (normal | ray.direction());

	if (DoubleCompare(tmp, 0) < 0)
	{
		//中心到它的向量
		Vector v = position - ray.origin();
		//平面的参数方程为：p(s,t) = origin + s*dx + t*dy 0<=s,t<=1

		Vector tmp1 = (dy ^ ray.direction());
		float s = -(tmp1 | v) / (tmp1 | dx);

		Vector tmp2 = (dx ^ ray.direction());
		float t = -(tmp2 | v) / (tmp2 | dy);

		float bestTime = (normal | v) / (normal | ray.direction());


		if (DoubleCompare(s, 0) >= 0 && DoubleCompare(s, 1) <= 0 && DoubleCompare(t, 0) >= 0 && DoubleCompare(t, 1) <= 0)
		{
			dist = bestTime;
			return true;
		}
	}
	return false;


	//float A = O_plane.getA();
	//float B = O_plane.getB();
	//float C = O_plane.getC();
	//float D = O_plane.getD();
	//float a = ray.direction().X;
	//float b = ray.direction().Y;
	//float c = ray.direction().Z;
	//if (ray.direction()|(O_plane.getNormal()) >= 0)
	//	return false;
	//float divisor = A * a + B * b + C * c;

	//float t = -(A * ray.origin().X + B * ray.origin().Y + C * ray.origin().Z + D) / divisor;
	//Vector intersect = Vector(t * a + ray.origin().X, t * b + ray.origin().Y, t * c + ray.origin().Z);

		
	return true;
	//double d = (m_Plane.N|o_ray.direction());
	//if (d != 0)
	//{
	//	double Pdist = -((m_Plane.N|o_ray.origin()) + m_Plane.D) / d;
	//	if (Pdist > 0)
	//	{
	//		if (Pdist < dist)
	//		{
	//			dist = Pdist;
	//			return true;
	//		}
	//	}
	//}
	//return false;
}



