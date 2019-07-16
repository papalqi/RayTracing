#pragma once
#include "render.h"

class WhittedRender :public Render
{
public:
	WhittedRender(Options& options) :Render(options) {};
	Vector Shader(const Ray p_ray, uint32_t depth, bool test = false) { return 0; };
	//Vector Shader(
	//	const Vector& orig, const Vector& dir,
	//	const std::vector<std::unique_ptr<Object>>& objects,
	//	const std::vector<std::unique_ptr<Light>>& lights,
	//	uint32_t depth,
	//	bool test = false);
};