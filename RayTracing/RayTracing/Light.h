#pragma once
#include "CoreMinimal.h"
class Light
{
public:
	Light(const Vector &p, const Vector &i) : position(p), intensity(i) {}
	Vector position;
	Vector intensity;
};
