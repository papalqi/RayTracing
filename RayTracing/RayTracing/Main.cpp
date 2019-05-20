#include "render.h"


#define OUT_IMAGE_SIZEX 640
#define OUT_IMAGE_SIZEY 480

int main()
{
	std::vector<std::unique_ptr<Object>> objects;
	std::vector<std::unique_ptr<Light>> lights;

	Sphere *sph1 = new Sphere(Vector(-1, 0, -12), 2);
	sph1->materialType = DIFFUSE_AND_GLOSSY;
	sph1->diffuseColor = Vector(0.6, 0.7, 0.8);
	Sphere *sph2 = new Sphere(Vector(0.5, -0.5, -8), 1.5);
	sph2->ior = 1.5;
	sph2->materialType = REFLECTION_AND_REFRACTION;

	objects.push_back(std::unique_ptr<Sphere>(sph1));
	objects.push_back(std::unique_ptr<Sphere>(sph2));

	Vector verts[4] = { {-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16} };
	uint32_t vertIndex[6] = { 0, 1, 3, 1, 2, 3 };
	Vector2D st[4] = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
	MeshTriangle *mesh = new MeshTriangle(verts, vertIndex, 2, st);
	mesh->materialType = DIFFUSE_AND_GLOSSY;

	objects.push_back(std::unique_ptr<MeshTriangle>(mesh));

	lights.push_back(std::unique_ptr<Light>(new Light(Vector(-20, 70, 20), 0.5)));
	lights.push_back(std::unique_ptr<Light>(new Light(Vector(30, 50, -12), 1)));

	Options options;
	options.width = OUT_IMAGE_SIZEX;
	options.height = OUT_IMAGE_SIZEY;
	options.fov = 90;
	options.backgroundColor = LightSkyBlue;
	options.maxDepth = 5;
	options.bias = 0.00001;
	options.renderType = Whitted;

	Render whittedRender(options);
	whittedRender.Rendering( objects, lights);

	return 0;
}
