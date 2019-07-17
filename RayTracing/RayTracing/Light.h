/*!
 * \file Light.h
 *
 * \author papalqi.huang
 * \date  BaseLighting
 *基础的灯光，可视为PointLighting
 * 
 */
#pragma once

#include "CoreMinimal.h"
#include "Object.h"
enum LightClass
{
	point ,
	Shpere,
	Rect,
};

class BaseLighting:public Object
{
public:
	BaseLighting();
	BaseLighting(const Vector &p, const Vector &i);
	 virtual bool intersect(Ray o_ray, float& dist) { return false; };
	 //点光源是无法进行交集计算的
	 virtual bool intersect(const Vector&, const Vector&, float&, uint32_t&, Vector2D&) const { return false; };
	 virtual int getType() {return point;};
	 virtual Vector getNormal(Vector& p_Pos) { return Vector::ZeroVector; };
	 virtual LightClass GetLightType() { return LightType; };
public:
	Vector position;
	Vector intensity;
	float Size=5;
	LightClass LightType;
};
#define   PointLighting  BaseLighting ;