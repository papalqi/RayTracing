#pragma once

#include "CoreMinimal.h"
#include "Camera.h"
#include"Object.h"
#include"Shpere.h"
#include"material.h"
#include "ObjectList.h"
#include "Light.h"
#include <cstdio> 
#include <cstdlib> 
#include <memory> 
#include <vector> 
#include <utility> 
#include <cstdint> 
#include <iostream> 
#include <fstream> 
#include <cmath> 
#include "Camera.h"
#include "MeshTriangle.h"
#include "Color.h"
#include "Plane.h"
#include "MaterialMath.h"
const float kInfinity = std::numeric_limits<float>::max();

enum RENDERTYPE
{
	RayCast,
	Whitted,
	distributed,
	pathTracing,
	BidirectionalPathTracing,
	Metropolis,

};

struct Options
{
	uint32_t width;
	uint32_t height;
	float fov;
	float imageAspectRatio;
	uint8_t maxDepth;
	Vector backgroundColor;
	float bias;
	RENDERTYPE renderType;

};


class Render
{
public:
	Render(Options& options) { this->options = &options; }
	//判断是否追踪到有物体
	void SetSphere(Vector position ,float rad,material::Refl_t mtype,Vector ks, Vector ka, Vector kd ,float shiness,string name);


	void SetPlane(Vector position, Vector  normal, Vector DX, Vector DY, float rad,material::Refl_t mtype, Vector ks, Vector ka, Vector kd, float shiness, string name);

	virtual bool FindNearest(const Ray p_ray, float& tNear, Object** hitObject);
	//只针对Whitted 和RayCasting ，因为只有点光源
	float GetShadowFact(BaseLighting* mlt, Vector position, Vector& p_Dir);
	//所有的渲染入口
	virtual void Rendering();
	Color GetDirectColor(Ray p_ray,BaseLighting* light, Object* hitObject, Vector hitPoint, Vector normal);

	float GetShadowFact(BaseLighting* mlt, Vector position, Vector& p_Dir, float p_Sample, float p_SampleRange);
	Color GetDiffuseColor(material m_material, BaseLighting* m_light, Vector normal, Vector position, Object* obj);
	Color GeSpecularColor(material m_material, BaseLighting* m_light, Vector normal, Vector position, Object* obj, Vector Dir);
	virtual	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false) = 0;
	//输出图片
	void OutputImage(string paths, string name, Vector*);
	//初始化场景
	virtual void InitScene();
	void AddLight(LightClass ,Vector p,Color cor,float size,string name);
	inline bool Refract(const Vector i, const Vector n, float eta, Vector& refractDirection)
	{
		float ndoti = (n | i), k = 1.0f - eta * eta * (1.0f - ndoti * ndoti);
		if (k >= 0.0f) {
			refractDirection = eta * i - n * (eta * ndoti + sqrt(k));
			return true;
		}
		// total internal reflection. There is no refraction in this case
		else return false;
	}
	inline Vector Reflect(const Vector i, const Vector n)
	{
		return i - 2.0f * (n| i) * n;
	}
	inline float getFresnelIndex(float ni, float nt, float cosTheta)
	{
		//fresnel反射系数的schlick近似
		double f0 = (ni - nt) / (ni + nt);
		f0 *= f0;
		double schlick = f0 + (1 - f0) * pow(1.0 - cosTheta, 5);

		return schlick;
	}

public:
	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<BaseLighting>> lights;
protected:
	float Const_Max_Sample;
	Options* options;
};






