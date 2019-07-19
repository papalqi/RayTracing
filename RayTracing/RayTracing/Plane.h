#pragma once
#include "CoreMinimal.h"

#include "Object.h"



class PlaneObj : public Object
{
public:
	
	PlaneObj(Vector position, Vector  normal, Vector dx, Vector dy);


	inline int getType() { return PLANE; }
	Color getColor(Vector& p_Pos);
	bool intersect(Ray o_ray, float& dist);
	inline Vector getNormal(Vector& p_Pos) { return normal; };

private:

	Vector dy; 
	Vector dx;
	Vector normal;
	Vector position;
};



