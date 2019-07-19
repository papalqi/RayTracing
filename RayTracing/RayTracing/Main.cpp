#include "render.h"
#include"RayTracingDifine.h"
#include "RayCasting.h"
#include "WhittedRender.h"
#include "DistributedRender.h"
#include "BidirectionalPathTracingRender.h"
#include "PathTracingRender.h"
#include "MetropolisRender.h"

int main(int argc, char* argv[])
{
	Options options;
	options.width = OUT_IMAGE_SIZEX;
	options.height = OUT_IMAGE_SIZEY;
	options.fov = 90;
	options.backgroundColor = LightSkyBlue;
	options.maxDepth = 5;
	options.bias = 0.00001f;
	Render * FuckingRender=nullptr;
	if (argc != 2)
	{
		//FuckingRender = new DistributedRender(options);
		FuckingRender = new PathTracingRender(options);
		//FuckingRender = new PathTracingRender(options);
		//FuckingRender = new DistributedRender(options);
		//FuckingRender=new RayCastingRender(options);
	}
	else if (strcmp(argv[1], "Whitted") == 0)
	{
		FuckingRender = new WhittedRender(options);

	}
	else if (strcmp(argv[1], "distributed ") == 0)
	{
		FuckingRender = new DistributedRender(options);

	}
	else if (strcmp(argv[1], "pathTracing") == 0)
	{
		FuckingRender = new PathTracingRender(options);

	}
	else if (strcmp(argv[1], "BidirectionalPathTracing") == 0)
	{
		FuckingRender = new BidirectionalPathTracingRender(options);

	
	}
	else if (strcmp(argv[1], "Metropolis") == 0)
	{
		FuckingRender = new MetropolisRender(options);

	}
	if (FuckingRender == nullptr)
		return 0;
	
	FuckingRender->InitScene();
	FuckingRender->Rendering();

	return 0;
}
