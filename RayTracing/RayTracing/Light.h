#pragma once
#include "CoreMinimal.h"
enum LightClass
{
	point ,
	Shpere,
	Rect,
};
class Light
{
public:
	Light(const Vector &p, const Vector &i) : position(p), intensity(i) {}
	Vector position;
	Vector intensity;
	LightClass LightType;
};
