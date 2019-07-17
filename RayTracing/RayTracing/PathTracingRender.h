#pragma once
#include "render.h"
class PathTracingRender :public Render
{
public:
	PathTracingRender(Options& options) :Render(options) {};
	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false) ;


};
