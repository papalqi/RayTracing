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
	//�ж��Ƿ�׷�ٵ�������
	virtual bool trace(
		const Vector& orig, const Vector& dir,
		const std::vector<std::unique_ptr<Object>>& objects,
		float& tNear, uint32_t& index, Vector2D& uv, Object** hitObject);

	virtual bool FindNearest(const Ray p_ray, float& tNear, Object** hitObject);
	//ֻ���Whitted ��RayCasting ����Ϊֻ�е��Դ
	float GetShadowFact(BaseLighting* mlt, Vector position, Vector& p_Dir);
	//���е���Ⱦ���
	virtual void Rendering();



	Color GetDiffuseColor(material m_material, BaseLighting* m_light, Vector normal, Vector position, Object* obj);
	Color GeSpecularColor(material m_material, BaseLighting* m_light, Vector normal, Vector position, Object* obj, Vector Dir);
	virtual	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false) = 0;
	//���ͼƬ
	void OutputImage(string paths, string name, Vector*);
	//��ʼ������
	void InitScene();
	void AddLight(LightClass ,Vector p,Color cor,float size);

public:
	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<BaseLighting>> lights;
protected:
	float Const_Max_Sample;
	Options* options;
};






