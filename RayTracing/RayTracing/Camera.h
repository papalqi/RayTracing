#pragma once
#include "CoreMinimal.h"
#include "Random.h"
#include "Ray.h"
struct Ray;


class Camera
{
public:
	Camera():m_Eye(0,0,-5),m_Target(0,0,0),m_Dist(5.0),m_Width(8.0),m_Height(6.0),m_Ratio(0.01){}
	Camera(Vector p_Eye, Vector p_Target, float p_Dist, float p_Width, float p_Height, float p_Ratio) :
		m_Eye(p_Eye), m_Target(p_Target), m_Dist(p_Dist), m_Width(p_Width), m_Height(p_Width), m_Ratio(p_Ratio) {}
	inline Vector getEye() { return m_Eye; }
	Vector getDir(float x, float y)//像素坐标，整数，也可以在超采样中取小数
	{
		Vector dir = m_Target - m_Eye;
		(dir).Normalize();
		dir = m_Dist* dir  ;
		//return dir;
		m_x = dir^(Vector(0, 1, 0));
		(m_x).Normalize();
		m_y = m_x^(dir);
		(m_y).Normalize();
		Vector ret = dir + (x * m_Ratio - 0.5 * m_Width) * m_x + (y * m_Ratio - 0.5 * m_Height) * m_y;
		return ret;
	}
	inline void setRV(float p_R, float p_V) { m_R = p_R; m_V = p_V; }
	Ray getRandRay(float x, float y)//景深，获取光圈内随机的一个光线，用以计算颜色
	{
		float x0, y0;
		do
		{
			x0 = 2.0 * (float)rand() / RAND_MAX - 1.0;
			y0 = 2.0 * (float)rand() / RAND_MAX - 1.0;
		} while (x0 * x0 + y0 * y0 > 1);
		x0 *= m_R;
		y0 *= m_R;
		Vector randPos = m_Eye + x0 * m_x + y0 * m_y;
		Vector dir = getDir(x, y);
		Vector focusPos = m_Eye + m_V  * (1.0 / m_Dist) * dir;
		Vector rayDir = focusPos - randPos;
		(rayDir).Normalize();
		return Ray(randPos, rayDir);
	}
private:
	Vector m_Eye;
	Vector m_Target;
	float m_Dist;
	float m_Width;
	float m_Height;
	Vector m_x;//投影平面的x方向基向量
	Vector m_y;//投影平面的y方向基向量
	float m_Ratio;//从整点像素到三维坐标系的缩放比，即图像单位像素对应投影平面在坐标系中的长度

	float m_R;//光圈的半径
	float m_V;//眼睛到焦平面的距离
};

class FCamera
{
public:
	FCamera();
	Ray getRay(double x, double y);
	void lookAt(double ex, double ey, double ez, double lx, double ly, double lz, double upx, double upy, double upz);
	void perspective(double fov, double aspect, double near, double far);
	void calcViewPort(double fov, double width, double height);
	void rotate(double angle, const Vector& axis);

	double width, height;//视口宽高
	double nearPlane, farPlane;
private:
	Vector origin;
	Vector direction;

	double fov;//垂直方向的 filed of view
	double rotateX, rotateY;
	Vector front, up, right;
	Vector view_x, view_y, view_z;
};

