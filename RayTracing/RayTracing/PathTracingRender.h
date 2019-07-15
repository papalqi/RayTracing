#pragma once
#include "render.h"
class PathTracingRender :public Render
{
public:

	PathTracingRender(Options& options) :Render(options) {};
	Vector Shader(
		const Vector& orig, const Vector& dir,
		const std::vector<std::unique_ptr<Object>>& objects,
		const std::vector<std::unique_ptr<Light>>& lights,
		uint32_t depth,
		bool test = false);
};
