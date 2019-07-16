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


public:
	Vector position;
	Vector intensity;
	float Size=5;
	LightClass LightType;
};
