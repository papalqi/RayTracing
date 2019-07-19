#pragma once
#include "Light.h"
#include "Plane.h"
class RectLighting :public BaseLighting
{
public:
	RectLighting() {};
	RectLighting(Vector position, Vector Normal,Vector ddx, Vector ddy, Vector emission) 
	{
		dx = ddx;
		dy = ddy;
		this->position = position;
		normal = (ddx ^ ddy);
		area = ddx.Size();
		this->normal = (Normal).GetSafeNormal();
	
	};

	virtual bool intersect(Ray o_ray, float& dist);
	//���Դ���޷����н��������

	virtual int getType() { return LightClass::Rect; };

	inline Vector getNormal(Vector& p_Pos) { return normal; }

	//OBJPlane m_Plane;
	Vector normal;
	Vector dx;
	float area;
	Vector dy;
	Vector O_H;//ԭ����ƽ���ϵĴ���

};

bool RectLighting::intersect(Ray ray, float& dist)
{
	float tmp = (normal|ray.direction());

	if (DoubleCompare(tmp, 0) < 0)
	{
		//���ĵ���������
		Vector v = position - ray.origin();
		//ƽ��Ĳ�������Ϊ��p(s,t) = origin + s*dx + t*dy 0<=s,t<=1

		Vector tmp1 = (dy^ ray.direction());
		float s = -(tmp1| v) / (tmp1| dx);

		Vector tmp2 = (dx^ ray.direction());
		float t = -(tmp2| v) / (tmp2|dy);

		float bestTime = (normal|v) / (normal| ray.direction());
		

		if (DoubleCompare(s, 0) >= 0 && DoubleCompare(s, 1) <= 0 && DoubleCompare(t, 0) >= 0 && DoubleCompare(t, 1) <= 0)
		{
			dist = bestTime;
			return true;
		}
	}
	return false;
}
