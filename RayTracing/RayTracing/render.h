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
#include "MeshTriangle.h"
#include "Color.h"
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
	Render(Options  &options) {this->options = &options;}

	virtual bool trace(
	const Vector &orig, const Vector &dir,
	const std::vector<std::unique_ptr<Object>> &objects,
	float &tNear, uint32_t &index, Vector2D &uv, Object **hitObject);


	//所有的渲染入口
virtual void Rendering(const std::vector<std::unique_ptr<Object>> &objects,
	const std::vector<std::unique_ptr<Light>> &lights);

virtual	Vector Shader(
	const Vector& orig, const Vector& dir,
	const std::vector<std::unique_ptr<Object>>& objects,
	const std::vector<std::unique_ptr<Light>>& lights,
	uint32_t depth,
	bool test = false) = 0;
//输出图片
void OutputImage(string paths, string name, Vector *);
//初始化场景
void InitScene();

public:
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Light>> lights;
protected:

	Options *options;
};






