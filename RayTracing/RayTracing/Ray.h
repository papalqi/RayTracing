#pragma once
#include "CoreMinimal.h"
struct Ray
{
	Vector m_origin;
	Vector m_direction;

	Ray() {}
	Ray(const Vector& a, const Vector& b) { m_origin = a; m_direction = b; }
	Vector origin() const { return m_origin; }
	Vector direction() const { return m_direction.GetSafeNormal(); }
	Vector point_at_parameter(float t) const { return m_origin + t * m_direction; }
};