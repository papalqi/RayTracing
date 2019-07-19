#pragma once
#include "render.h"
class PathTracingRender :public Render
{
public:
	PathTracingRender(Options& options) :Render(options) {};
	Vector Shader(const Ray p_ray, uint32_t depth) ;
	Vector Shader(const Ray p_ray, uint32_t depth, bool) { return 0; };
	//使用lambertian着色
	bool SampleDiffuse(Ray p_ray,Vector normal, Vector position,material attenuation, Ray& out_Ray);
	bool SampleMetal(Ray p_ray, Vector normal, Vector position, material attenuation, Ray& out_Ray);

	Ray mcSelect(Ray  ray, material mt, Vector Normal,Vector HitPosition, float& survival);
	virtual void Rendering();
	virtual void InitScene();
};
