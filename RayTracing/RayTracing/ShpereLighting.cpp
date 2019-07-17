#include "ShpereLighting.h"

ShpereLighting::ShpereLighting(Vector o_Position, float rad, Color SCol)
{
	position = o_Position;
	center = position;
	radius = rad;
	radius2 = radius;
	intensity = SCol;
	this->m_Material.setColor(intensity);
	LightType = Shpere;
	bLight = true;

}

ShpereLighting::ShpereLighting()
{
	position = 0;
	radius = 1;
	radius2 = radius;
	center = position;
	intensity = Vector(1, 1, 1);
	this->m_Material.setColor({ 1, 1, 1 });
	LightType = Shpere;
	bLight = true;

}

bool ShpereLighting::intersect(Ray o_ray, float& dist)
{
	Vector v = o_ray.origin() - center;
	double b = -(v | o_ray.direction());
	double det = (b * b) - (v | v) + radius2;
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

