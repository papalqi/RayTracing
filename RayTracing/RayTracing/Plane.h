#pragma once
#include "CoreMinimal.h"

#include "Object.h"
class Plane
{
public:
	Plane();
	~Plane();

public:
	Vector point;
	Vector normal;
	
	bool intersect(
		const Ray & ray, const double min_t, double & t, Vector & n) const;
};

