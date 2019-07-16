#pragma once
#include "CoreMinimal.h"

#include "Object.h"

class plane
{
public:
	plane() : N(0, 0, 0), D(0) {};
	plane(Vector p_Normal, double p_D) : N(p_Normal), D(p_D) {};
	union
	{
		struct
		{
			Vector N;
			double D;
		};
		double cell[4];
	};
};

class PlaneObj : public Object
{
public:
	
	PlaneObj(Vector p_Normal, double p_D);
	inline Vector& getNormal() { return m_Plane.N; }
	inline double getD() { return m_Plane.D; }
	inline int getType() { return PLANE; }
	Color getColor(Vector& p_Pos);
	bool intersect(const Vector& orig, const Vector& dir, float& tnear, uint32_t& index, Vector2D& uv) const;
	bool intersect(Ray o_ray, float& dist);
	//bool H_IntersectBox(BoundingBox&);
	inline Vector getNormal(Vector& p_Pos) { return m_Plane.N; };
	//inline BoundingBox getBoundingBox() { return BoundingBox(Vector(-10000, -10000, -10000), vector3(20000, 20000, 20000)); }

private:
	plane m_Plane;
	Vector x_Dir;
	Vector y_Dir;
	Vector O_H;//原点在平面上的垂足
};



