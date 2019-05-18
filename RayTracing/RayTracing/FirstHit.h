#pragma once


#include "Ray.h"
#include "Object.h"
#include "CoreMinimal.h"
#include <vector>
#include <memory>



bool first_hit(
	const Ray & ray,
	const double min_t,
	const std::vector< std::shared_ptr<Object> > & objects,
	int & hit_id,
	double & t,
	Vector & n)
{
	return true;
}


