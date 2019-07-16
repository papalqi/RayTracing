#pragma once
#include "render.h"

class WhittedRender :public Render
{
public:
	WhittedRender(Options& options) :Render(options) {};
	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false);

};