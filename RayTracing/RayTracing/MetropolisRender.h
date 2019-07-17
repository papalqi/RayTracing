#pragma once
#include "render.h"
class MetropolisRender :public Render
{
public:
	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false) { return 0; };

	MetropolisRender(Options& options) :Render(options) {};
	virtual	Vector Shader(
		const Vector& orig, const Vector& dir,
		const std::vector<std::unique_ptr<Object>>& objects,
		const std::vector<std::unique_ptr<BaseLighting>>& lights,
		uint32_t depth,
		bool test = false);
};