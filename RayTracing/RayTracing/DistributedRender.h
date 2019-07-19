#pragma once
#include "render.h"


class DistributedRender :public Render
{
public:
	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false);
	Vector Shader(const Ray p_ray, uint32_t depth, float p_Sample, float p_SampleRange,bool test = false);
	DistributedRender(Options& options) :Render(options) {};

};



