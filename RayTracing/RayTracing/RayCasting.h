#pragma once
#include "render.h"


class RayCastingRender :public Render
{
public:

	RayCastingRender(Options& options):Render(options){};
	virtual	Vector Shader(
		const Vector& orig, const Vector& dir,
		const std::vector<std::unique_ptr<Object>>& objects,
		const std::vector<std::unique_ptr<Light>>& lights,
		uint32_t depth,
		bool test = false);
};